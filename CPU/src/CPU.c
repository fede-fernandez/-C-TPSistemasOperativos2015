#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"

#define RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU "cfgCPU"
#define RUTA_DE_ARCHIVO_DE_LOGS_CPU "logsCPU"
#define LOGS_ACTIVADOS 0
#define DEBUG 1




//Estructura Hilo CPU
typedef struct
{
	int idCPU;
	tipoConfigCPU* configuracionCPU;
	t_log* logCPU;
}t_hiloCPU;

void* unCPU(t_hiloCPU* datosCPU);

int main(void)
{
	//Parametros para crear un hiloCPU: idCPU, archivoDeConfiguracion, archivoDeLogs
	t_hiloCPU datosCPU;

	//Carga de archivo de configuracion
	datosCPU.configuracionCPU = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU);

	//Creacion archivo de log
	if(LOGS_ACTIVADOS == 1)
	{
		datosCPU.logCPU = log_create(RUTA_DE_ARCHIVO_DE_LOGS_CPU, "CPU", 1, LOG_LEVEL_TRACE);
	}

	//Crea tantos "CPUs" (hilos), especificado en el archivo de configuracion
	pthread_t* hiloCPU[datosCPU.configuracionCPU->cantidadDeHilos];
	int i;
	for(i = 0; i < datosCPU.configuracionCPU->cantidadDeHilos; i++)
	{
		datosCPU.idCPU = i;
		pthread_create(&hiloCPU[i], NULL, unCPU, &datosCPU);
		pthread_join(hiloCPU[i], NULL);
	}
	return 0;
}

//Hilo CPU
void* unCPU(t_hiloCPU* datosCPU)
{
	if(DEBUG == 1)
	{
		printf("idCPU: %i | CPU INICIADA\n", datosCPU->idCPU);
	}

	//Conexion a Planificador
	int socketParaPlanificador = crearSocket();
	conectarAServidor(socketParaPlanificador, datosCPU->configuracionCPU->ipPlanificador, datosCPU->configuracionCPU->puertoPlanificador);

	//Primer mensaje para planificador, diciendo que estoy online y me responde con el quantum, si quantum = 0 -> FIFO
	enviarMensaje(socketParaPlanificador, &datosCPU->idCPU, sizeof(datosCPU->idCPU));

	int quantum = 0;
	recibirMensajeCompleto(socketParaPlanificador, &quantum, sizeof(int));
	if(DEBUG == 1)
	{
		printf("idCPU: %i | QUANTUM RECIBIDO: %i\n", datosCPU->idCPU, quantum);
	}

	//Me trato de conectar con Memoria
	int socketParaMemoria = crearSocket();
	conectarAServidor(socketParaMemoria, datosCPU->configuracionCPU->ipMemoria, datosCPU->configuracionCPU->puertoMemoria);

	//LOG: CPU creada conectada/conectada
	if(LOGS_ACTIVADOS == 1)
	{
		log_trace(datosCPU->logCPU, "CPU ID: %i CREADA/CONECTADA A MEMORIA", datosCPU->idCPU);
	}

	//Espero a recibir tarea del planificador
	while(true)
	{
		tipoPCB* PCB = recibirPCB(socketParaPlanificador);

		if(DEBUG == 1)
		{
			printf("idCPU: %i | PCB RECIBIDO: ", datosCPU->idCPU);
			imprimirPCB(PCB);
		}

		//LOG: CPU recibe PCBs
		if(LOGS_ACTIVADOS == 1)
		{
			log_trace(datosCPU->logCPU, "CPU ID: %i PCB RECIBIDO. RUTA: %s | ESTADO: %c | PID: %i | INSPOINTER: %i", datosCPU->idCPU, PCB->ruta, PCB->estado, PCB->pid, PCB->insPointer, quantum);
		}

		//ejecutarPrograma(PCB, quantum, datosCPU->configuracionCPU->retardo, socketParaPlanificador, socketParaMemoria);
	} //ACORDATE DE REEMPLAZAR DATOSCPU->CONFIGBLABLA POR DATOS CPU UNICAMENTE Y MODIFICAR FUNCION

	liberarSocket(socketParaMemoria);
	liberarSocket(socketParaPlanificador);
	return 0;
}
