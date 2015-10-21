#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"

#define RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU "cfgCPU"
#define RUTA_DE_ARCHIVO_DE_LOGS_CPU "logsCPU"

void* CPU();


//Estructura Hilo CPU
typedef struct
{
	int idCPU;
	tipoConfigCPU* configuracionCPU;
	t_log* logCPU;
}t_hiloCPU;

/////////////////////REVISAR ESTRUCTURAS ASIGNACION DE STRINGS

int main(void)
{
	//Parametros para crear un hiloCPU: idCPU, archivoDeConfiguracion, archivoDeLogs
	t_hiloCPU* datosCPU;

	//Carga de archivo de configuracion
	datosCPU->configuracionCPU = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU);

	//Creacion archivo de log
	//CPU->logCPU = log_create(RUTA_DE_ARCHIVO_DE_LOGS_CPU, "CPU", 1, LOG_LEVEL_TRACE);

	//Crea tantos "CPUs" (hilos), especificado en el archivo de configuracion
	pthread_t hiloCPU;
	int i;
	for(i = 0; i < datosCPU->configuracionCPU->cantidadDeHilos; i++)
	{
		datosCPU->idCPU = i;
		pthread_create(&hiloCPU, NULL, unCPU, &datosCPU); //datosCPU lo tengo q pasar sin & para que funcione, pero tira warning si lo hago
	}


	free(CPU);
	return 0;
}

//Hilo CPU
void* unCPU(t_hiloCPU* datosCPU)
{
	int quantum = 0;
	//Conexion a Planificador
	int socketParaPlanificador = crearSocket();
	conectarAServidor(socketParaPlanificador, datosCPU->configuracionCPU->ipPlanificador, datosCPU->configuracionCPU->puertoPlanificador);

	//Primer mensaje para planificador, diciendo que estoy online y me responde con el quantum, si quantum = 0 -> FIFO
	enviarMensaje(socketParaPlanificador, datosCPU->idCPU, sizeof(int));
	recibirMensajeCompleto(socketParaPlanificador, &quantum, sizeof(int));

	//Me trato de conectar con Memoria
	int socketParaMemoria = crearSocket();
	conectarAServidor(socketParaMemoria, datosCPU->configuracionCPU->ipMemoria, datosCPU->configuracionCPU->puertoMemoria);

	//Espero a recibir tarea del planificador
	while(true)
	{
		tipoPCB* PCB;
		PCB = recibirPCB(socketParaPlanificador);

		//Me llega una tarea del planificador
		ejecutarPrograma(PCB, quantum, datosCPU->configuracionCPU->retardo, socketParaPlanificador, socketParaMemoria);
	}

	liberarSocket(socketParaMemoria);
	liberarSocket(socketParaPlanificador);
	return 0;
}
