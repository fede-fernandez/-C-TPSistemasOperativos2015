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
		pthread_create(&hiloCPU, NULL, CPU, &i);
	}
	destruirConfigCPU(configuracion);
	return EXIT_SUCCESS;
}

void* CPU(int idCPU)
{
	int quantum = 0;
	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION);

	//Conexion a Planificador
	int socketParaPlanificador = crearSocket();
	conectarAServidor(socketParaPlanificador, configuracion->ipPlanificador, configuracion->puertoPlanificador);

	//Primer mensaje para planificador, diciendo que estoy online y me responde con el quantum, si quantum = 0 -> FIFO
	//Falta desarrollar round robin, como tomo los segundos???
	enviarMensaje(socketParaPlanificador, &idCPU, sizeof(&idCPU));
	recibirMensajeCompleto(socketParaPlanificador, &quantum, sizeof(quantum));

	//Me trato de conectar con Memoria
	int socketParaMemoria = crearSocket();
	conectarAServidor(socketParaMemoria, configuracion->ipMemoria, configuracion->puertoMemoria);
	char* mensajeParaMemoria = "Respondeme";
	enviarMensaje(socketParaMemoria, mensajeParaMemoria, sizeof(mensajeParaMemoria));

	char* respuestaDeMemoria;
	recibirMensajeCompleto(socketParaMemoria, respuestaDeMemoria, sizeof(respuestaDeMemoria));
	if(respuestaDeMemoria != "Te respondo")
	{
		//Se cae este hilo, aviso a planificador
	}

	//Espero a recibir tarea del planificador
	while(true)
	{
		tipoPCB* PCB;
		PCB = recibirPCB(socketParaPlanificador);


		//Me llega una tarea del planificador
		tipoPCB PCBRespuesta;
		PCBRespuesta = *PCB;
		ejecutarPrograma(PCBRespuesta, quantum, configuracion->retardo, socketParaPlanificador, socketParaMemoria);
		enviarPCB(socketParaPlanificador, PCBRespuesta);
	}
	liberarSocket(socketParaMemoria);
	liberarSocket(socketParaPlanificador);
	destruirConfigCPU(configuracion);
	return NULL;
}
