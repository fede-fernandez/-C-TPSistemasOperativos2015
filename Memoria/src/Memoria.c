#include <stdio.h>
#include <stdlib.h>
#include "funcionesMemoria.h"
#include <commonsDeAsedio/cliente-servidor.h>
#include <commons/collections/list.h>
#include <commonsDeAsedio/select.h>
//-----------------------------------------------------------------
#include <sys/types.h>
#include <unistd.h>
//---------------------------------------------------------------
#define maxConexionesEntrantes 10
#define puertoSwap 7204

int main(void) {

//////////////////////////INICIALIZACION DE VARIABLES////////////////////////////////

	tipoConfigMemoria* configuracion = cargarArchivoDeConfiguracionDeMemoria("/home/utnso/Escritorio/cfgMemoria");

	int socketParaCpus = crearSocket();

	int socketParaSwap = crearSocket();

	int socketCpuEntrante;

	bool memoriaActiva = true;//,hayCpuParaConexion = false;

	//t_list* listaPrincipal = list_create();
	//t_list* listaFiltrada = list_create();

//--------------ACA EMPIEZA FERNILANDIA--------------------------
	t_list* listaRAM = list_create();
	t_list* listaTLB = list_create();
	t_list* listaAdministracionPaginas = list_create();

	fd_set listaPrincipal;
	fd_set listaFiltrada;

	FD_ZERO(&listaPrincipal);


tipoEstructuraMemoria datosMemoria;

	datosMemoria.listaRAM = listaRAM;

	datosMemoria.listaTLB = listaTLB;

	datosMemoria.socketSWAP = socketParaSwap;

	datosMemoria.maximoSocket = socketParaCpus;

	datosMemoria.configuracion = configuracion;

	datosMemoria.cpusATratar = &listaFiltrada;

	datosMemoria.administradorPaginas = listaAdministracionPaginas;

	setearEstructuraMemoria(&datosMemoria);

//-------------END OF FERNILANDIA-----------------------------------

	asociarAPuerto(socketParaCpus,configuracion->puertoDeEscucha);


/////////////////////////////////////////////////////////////////////////////////////


	while(memoriaActiva){

		FD_ZERO(&listaFiltrada);

		FD_SET(socketParaCpus,&listaFiltrada);

		//hayCpuParaConexion = filtrarListas(listaPrincipal,listaFiltrada);

		select(datosMemoria.maximoSocket+1,&listaFiltrada,NULL,NULL,NULL);

		if(FD_ISSET(socketParaCpus,&listaFiltrada)){
			socketCpuEntrante = crearSocketParaAceptarSolicitudes(socketParaCpus);
			FD_SET(socketParaCpus,&listaPrincipal);
			datosMemoria.maximoSocket = maximoEntre(datosMemoria.maximoSocket,socketCpuEntrante);
		}

		tratarPeticiones(&datosMemoria);
		//if(!list_is_empty(listaFiltrada))
		//tratarPeticiones(socketParaCpus,socketParaSwap,listaFiltrada, listaTLB, listaRAM, configuracion);

	}

	destruirConfigMemoria(configuracion);

	return EXIT_SUCCESS;
}
