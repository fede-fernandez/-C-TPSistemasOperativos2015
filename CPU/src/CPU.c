#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"
#include <commonsDeAsedio/cliente-servidor.h>

#define puertoParaPlanificador 7201//Esto es solo
#define puertoParaAdministrador 7202//para la entrega

int main(void) {

	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU("/home/utnso/Escritorio/cfgCPU");//ruta de ejemplo

	int socketParaPlanificador = crearSocket();

	int socketParaAdministrador = crearSocket();

	asociarAPuerto(socketParaPlanificador,puertoParaPlanificador);

	asociarAPuerto(socketParaAdministrador,puertoParaAdministrador);

	conectarAServidor(socketParaPlanificador,configuracion->ipPlanificador,configuracion->puertoPlanificador);

	char mensaje[30];

	recibirMensaje(socketParaPlanificador,&mensaje,sizeof(mensaje));

	printf("El comando recibido del Planificador es: %s\n",mensaje);

	liberarSocket(socketParaPlanificador);

	conectarAServidor(socketParaAdministrador,configuracion->ipMemoria,configuracion->puertoMemoria);

	enviarMensaje(socketParaAdministrador,&mensaje,sizeof(mensaje));

	liberarSocket(socketParaAdministrador);

	destruirConfigCPU(configuracion);

	getchar();

	return EXIT_SUCCESS;
}
