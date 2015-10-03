#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "funcionesCPU.h"
#include <commonsDeAsedio/cliente-servidor.h>


#define RUTA_DE_ARCHIVO_DE_CONFIGURACION "cfgCPU"

int main(void) {
	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION);

	bool cpuActiva = true;

	int socketParaPlanificador = crearSocket();
	int socketParaAdministrador = crearSocket();
	conectarAServidor(socketParaPlanificador,configuracion->ipPlanificador,configuracion->puertoPlanificador);
	conectarAServidor(socketParaAdministrador,configuracion->ipMemoria,configuracion->puertoMemoria);



	liberarSocket(socketParaPlanificador);
	liberarSocket(socketParaAdministrador);

	destruirConfigCPU(configuracion);
	return EXIT_SUCCESS;
}

int CPU()
{
	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION);
	tipoPCB PCB;
	ejecutarPrograma(PCB, configuracion->retardo);

	return EXIT_SUCCESS;
}
