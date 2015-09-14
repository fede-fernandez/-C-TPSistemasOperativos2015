#include <stdio.h>
#include <stdlib.h>

#include <commons/collections/list.h>

#include <commonsDeAsedio/cliente-servidor.h>

#include "estructurasSWAP.h"
#include "funcionesSWAP.h"
#include "huecosSWAP.h"

#define MAXIMAS_CONEXIONES_ENTRANTES 1

int main(void) {

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("/home/utnso/Escritorio/cfgSWAP");
	FILE* particion = inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);
	//t_list* listaDeHuecosUtilizados = inicializarListaDeHuecosUtilizados();

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




	destruirConfigSWAP(configuracion);
	fclose(particion);


	return EXIT_SUCCESS;
}
