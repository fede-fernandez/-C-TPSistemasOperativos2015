#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include <commonsDeAsedio/cliente-servidor.h>

#include "estructurasSWAP.h"
#include "funcionesSWAP.h"
#include "huecosSWAP.h"
#include "particionSWAP.h"


#define MAXIMAS_CONEXIONES_ENTRANTES 1


tipoInstruccion* crearInstruccion(char ins,int pid,int pag,char* testo){
	tipoInstruccion* aux = malloc(sizeof(tipoInstruccion));

	aux->instruccion = ins;
	aux->pid = pid;
	aux->nroPagina = pag;
	aux->texto = strdup(testo);

	return aux;
}

int main(void) {

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("cfgSWAP");
	t_list* listaDeHuecosUtilizados = inicializarListaDeHuecosUtilizados();

	FILE* particion = inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);

	//FILE* particion = fopen("swap","r+");


//	int socketParaAdministrador = crearSocket();
//	asociarAPuerto(socketParaAdministrador,configuracion->puertoDeEscucha);
//	escucharConexiones(socketParaAdministrador,MAXIMAS_CONEXIONES_ENTRANTES);
//	int administradorMemoria = crearSocketParaAceptarSolicitudes(socketParaAdministrador);
//
//
//	char mensaje[30];
//	recibirMensaje(administradorMemoria,&mensaje,sizeof(mensaje));
//	printf("El comando recibido del Administrador de Memoria es: %s\n",mensaje);
//
//	liberarSocket(administradorMemoria);
//	liberarSocket(socketParaAdministrador);

	list_destroy_and_destroy_elements(listaDeHuecosUtilizados,(void*)destruirHuecoUtilizado);
	destruirConfigSWAP(configuracion);
	fclose(particion);


	return EXIT_SUCCESS;
}

