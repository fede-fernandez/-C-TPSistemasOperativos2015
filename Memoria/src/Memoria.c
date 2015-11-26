#include <stdio.h>
#include <stdlib.h>
#include "funcionesMemoria.h"
#include <commonsDeAsedio/cliente-servidor.h>
#include <commons/collections/list.h>
#include <commonsDeAsedio/select.h>
#include <commonsDeAsedio/thread.h>
#include <sys/types.h>
#include <unistd.h>

#include "impresionesEnPantalla.h"
//---------------------------------------------------------------
#define maxConexionesEntrantes 10

int main(void) {

//////////////////////////INICIALIZACION DE VARIABLES////////////////////////////////

	tipoConfigMemoria* configuracion = cargarArchivoDeConfiguracionDeMemoria("cfgMemoria");

	int socketParaCpus = crearSocket();

	int socketParaSwap = crearSocket();

	int socketCpuEntrante;

	bool memoriaActiva = true;

//--------------ACA EMPIEZA FERNILANDIA--------------------------

	fd_set listaPrincipal;
	fd_set listaFiltrada;

	FD_ZERO(&listaPrincipal);
	FD_ZERO(&listaFiltrada);
	FD_SET(socketParaCpus,&listaPrincipal);

	datosMemoria = malloc(sizeof(tipoEstructuraMemoria));

	datosMemoria->socketSWAP = socketParaSwap;

	datosMemoria->maximoSocket = socketParaCpus;

	datosMemoria->configuracion = configuracion;

	datosMemoria->memoriaActiva = &memoriaActiva;

	datosMemoria->socketCpus = socketParaCpus;

	system("if [ -f logMemoria ]; then rm logMemoria\nfi");//Si ya existe el log lo borra

	datosMemoria->logDeMemoria = log_create("logMemoria","Administrador de Memoria",false,LOG_LEVEL_TRACE);//crearLoggerParaSeguimiento("logMemoria","Administrador de Memoria");

	setearEstructuraMemoria(datosMemoria);

//-------------END OF FERNILANDIA-----------------------------------

	asociarAPuerto(socketParaCpus,configuracion->puertoDeEscucha);

	conectarAServidor(socketParaSwap,configuracion->ipSWAP,configuracion->puertoSWAP);

	escucharConexiones(socketParaCpus,maxConexionesEntrantes);

	pthread_t hiloSignals,hiloTasaTLB;

	crearThread(&hiloTasaTLB,mostrarTasaTLBPeriodicamente,NULL);

	crearThread(&hiloSignals,funcionPrueba,datosMemoria);


/////////////////////////////////////////////////////////////////////////////////////


	while(memoriaActiva){

		listaFiltrada = listaPrincipal;

		select(datosMemoria->maximoSocket+1,&listaFiltrada,NULL,NULL,NULL);

		int var;
		for (var = 0; var <= datosMemoria->maximoSocket; var++) {

			if(FD_ISSET(var, &listaFiltrada)){

				if(var==datosMemoria->socketCpus){
					socketCpuEntrante = crearSocketParaAceptarSolicitudes(datosMemoria->socketCpus);
					FD_SET(socketCpuEntrante,&listaPrincipal);
					datosMemoria->maximoSocket = maximoEntre(datosMemoria->maximoSocket,socketCpuEntrante);
				}

				else 	tratarPeticion(var);
		}
	}

	}

	destruirLogger(datosMemoria->logDeMemoria);

	free(datosMemoria);

	liberarSocket(socketParaCpus);

	liberarSocket(socketParaSwap);

	destruirConfigMemoria(configuracion);

	return EXIT_SUCCESS;
}
