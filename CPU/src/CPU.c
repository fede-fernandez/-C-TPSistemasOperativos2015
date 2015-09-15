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


/******Comienzo de Actividad de CPU cuando Planificador envia mensaje*******/
	int programCounterActual = 0, instruccionBloqueante;
	char* instruccionActual;
	FILE* programa = fopen(PATH, "r"); //PATH me lo envía planificador por socket

	if(programa == NULL)
		printf("\nEl programa no existe o está vacío.\n");

	while(programCounterActual < PC) //PC me lo envía planificador por socket
	{
		fgets(instruccionActual, 80, programa);
		programCounterActual++;
	}

	while(fgets(instruccionActual, 80, programa) != NULL)
	{
		instruccionBloqueante = ejecutarInstruccion(instruccionActual, pID);
		//pID me lo envia el planificador por socket
		if(instruccionBloqueante == 1)
		{
			sleep(configuracion->retardo);
			break;
		}
		sleep(configuracion->retardo);
	}
/******Fin de Actividad de CPU cuando Planificador envia mensaje*******/
	return EXIT_SUCCESS;
}
