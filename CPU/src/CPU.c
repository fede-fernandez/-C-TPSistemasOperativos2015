#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"

#define RUTA_DE_ARCHIVO_DE_CONFIGURACION "cfgCPU"

void* CPU();

int main(void) {

	//Carga de archivo de configuracion
	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION);

	//Crea tantos "CPUs" (hilos), especificado en el archivo de configuracion
	pthread_t hiloCPU;
	int i;
	for(i = 0; i < configuracion->cantidadDeHilos; i++)
	{
		pthread_create(&hiloCPU, NULL, CPU, &i);
	}


	destruirConfigCPU(configuracion);
	return 0;
}

//Hilo CPU
void* CPU(int idCPU)
{
	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION);

	int quantum = 0;
	//Conexion a Planificador
	int socketParaPlanificador = crearSocket();
	conectarAServidor(socketParaPlanificador, configuracion->ipPlanificador, configuracion->puertoPlanificador);

	//Primer mensaje para planificador, diciendo que estoy online y me responde con el quantum, si quantum = 0 -> FIFO
	enviarMensaje(socketParaPlanificador, &idCPU, sizeof(&idCPU));
	recibirMensajeCompleto(socketParaPlanificador, &quantum, sizeof(quantum));

	//Me trato de conectar con Memoria
	int socketParaMemoria = crearSocket();
	conectarAServidor(socketParaMemoria, configuracion->ipMemoria, configuracion->puertoMemoria);

	//Espero a recibir tarea del planificador
	while(true)
	{
		tipoPCB* PCB;
		PCB = recibirPCB(socketParaPlanificador);

		//Me llega una tarea del planificador
		ejecutarPrograma(PCB, quantum, configuracion->retardo, socketParaPlanificador, socketParaMemoria);
	}

	liberarSocket(socketParaMemoria);
	liberarSocket(socketParaPlanificador);
	destruirConfigCPU(configuracion);
	return NULL;
}
