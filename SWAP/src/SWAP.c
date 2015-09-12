#include <stdio.h>
#include <stdlib.h>
#include "funcionesSWAP.h"
#include <commonsDeAsedio/cliente-servidor.h>

int main(void) {

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("/home/utnso/Escritorio/cfgSWAP");

	//FILE* particion = inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);

	t_list* listaDeHuecosLibres = inicializarListaDeHuecosLibres(configuracion->cantidadDePaginas);

	int socketParaAdministrador = crearSocket();

	asociarAPuerto(socketParaAdministrador,configuracion->puertoDeEscucha);

	escucharConexiones(socketParaAdministrador,1);

	int administradorMemoria = crearSocketParaAceptarSolicitudes(socketParaAdministrador);

	char mensaje[30];

	recibirMensaje(administradorMemoria,&mensaje,sizeof(mensaje));

	printf("El comando recibido del Administrador de Memoria es: %s\n",mensaje);

	liberarSocket(administradorMemoria);

	liberarSocket(socketParaAdministrador);

	destruirConfigSWAP(configuracion);

	//fclose(particion);

	getchar();

	return EXIT_SUCCESS;
}
