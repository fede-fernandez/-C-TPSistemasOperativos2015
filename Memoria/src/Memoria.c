#include <stdio.h>
#include <stdlib.h>
#include "funcionesMemoria.h"
#include <commonsDeAsedio/cliente-servidor.h>
#include <commons/collections/list.h>
#include <commonsDeAsedio/select.h>
#include <commonsDeAsedio/thread.h>
#include "funcionesSeñales.h"
//-----------------------------------------------------------------
#include <sys/types.h>
#include <unistd.h>
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
	//t_list* listaRAM = list_create();
	//t_list* listaTLB = list_create();
	//t_list* listaAdministracionPaginas = list_create();

	fd_set listaPrincipal;
	fd_set listaFiltrada;

	FD_ZERO(&listaPrincipal);
	FD_ZERO(&listaFiltrada);
	FD_SET(socketParaCpus,&listaPrincipal);

	tipoEstructuraMemoria* datosMemoria = malloc(sizeof(tipoEstructuraMemoria));

	//datosMemoria->listaRAM = listaRAM;

	//datosMemoria->listaTLB = listaTLB;

	datosMemoria->socketSWAP = socketParaSwap;

	datosMemoria->maximoSocket = socketParaCpus;

	datosMemoria->configuracion = configuracion;

	//datosMemoria->cpusATratar = &listaFiltrada;

	//datosMemoria->listaCpus = &listaPrincipal;

	//datosMemoria->administradorPaginas = listaAdministracionPaginas;

	datosMemoria->memoriaActiva = &memoriaActiva;

	datosMemoria->socketCpus = socketParaCpus;

	setearEstructuraMemoria(datosMemoria);

//-------------END OF FERNILANDIA-----------------------------------

	asociarAPuerto(socketParaCpus,configuracion->puertoDeEscucha);

	conectarAServidor(socketParaSwap,configuracion->ipSWAP,configuracion->puertoSWAP);

	escucharConexiones(socketParaCpus,maxConexionesEntrantes);

	//pthread_t hiloSignals;

	//crearThread(&hiloSignals,funcionPrueba,datosMemoria);

	/*socketCpuEntrante = crearSocketParaAceptarSolicitudes(socketParaCpus);
	FD_SET(socketParaCpus,&listaPrincipal);
	datosMemoria->maximoSocket = maximoEntre(datosMemoria->maximoSocket,socketCpuEntrante);*/


/////////////////////////////////////////////////////////////////////////////////////


	while(memoriaActiva){

		//FD_ZERO(&listaFiltrada);

		listaFiltrada = listaPrincipal;

		select(datosMemoria->maximoSocket+1,&listaFiltrada,NULL,NULL,NULL);

		/*if(FD_ISSET(socketParaCpus,&listaFiltrada)){
			socketCpuEntrante = crearSocketParaAceptarSolicitudes(socketParaCpus);
			FD_SET(socketParaCpus,&listaPrincipal);
			FD_SET(socketCpuEntrante,&listaFiltrada);
			datosMemoria->maximoSocket = maximoEntre(datosMemoria->maximoSocket,socketCpuEntrante);
		}*/

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


	destruirConfigMemoria(configuracion);

	return EXIT_SUCCESS;
}
