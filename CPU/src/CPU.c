#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"


#define RUTA_DE_ARCHIVO_DE_CONFIGURACION "cfgCPU"

void* CPU();

int main(void) {
	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION);
	pthread_t hiloCPU;
	int i;
	for(i=0; i < configuracion->cantidadDeHilos; i++)
	{
		pthread_create(&hiloCPU, NULL, CPU, NULL);
	}
	destruirConfigCPU(configuracion);
	return EXIT_SUCCESS;
}

void* CPU()
{
	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION);
	int socketParaPlanificador = crearSocket();
	while(true)
	{
		conectarAServidor(socketParaPlanificador,configuracion->ipPlanificador,configuracion->puertoPlanificador);
	}
	tipoPCB PCB;
	ejecutarPrograma(PCB, configuracion->retardo);
	liberarSocket(socketParaPlanificador);
	destruirConfigCPU(configuracion);
	return NULL;
}
