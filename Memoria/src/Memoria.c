#include <stdio.h>
#include <stdlib.h>
#include "funcionesMemoria.h"
#include <commonsDeAsedio/cliente-servidor.h>

#define puertoCpus 7203
#define puertoSwap 7204

int main(void) {

	tipoConfigMemoria* configuracion = cargarArchivoDeConfiguracionDeMemoria("/home/utnso/Escritorio/cfgMemoria");

	int socketParaCpus = crearSocket();

	int socketParaSwap = crearSocket();

	asociarAPuerto(socketParaCpus,puertoCpus);

	asociarAPuerto(socketParaSwap,puertoSwap);

	escucharConexiones(socketParaCpus,1);

	int cpu = crearSocketParaAceptarSolicitudes(socketParaCpus);

	char mensaje[30];

	recibirMensaje(cpu,&mensaje,sizeof(mensaje));

	printf("El comando recibido de la cpu es: %s\n",mensaje);

	liberarSocket(cpu);

	liberarSocket(socketParaCpus);

	conectarAServidor(socketParaSwap,configuracion->ipSWAP,configuracion->puertoSWAP);

	enviarMensaje(socketParaSwap,&mensaje,sizeof(mensaje));

	liberarSocket(socketParaSwap);

	destruirConfigMemoria(configuracion);

	getchar();

	return EXIT_SUCCESS;
}
