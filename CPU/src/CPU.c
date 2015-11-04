#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"

int main(void)
{
	//Parametros para crear un hiloCPU: idCPU, archivoDeConfiguracion, archivoDeLogs, socketParaPlanificador, socketParaMemoria
	t_hiloCPU datosCPU;

	//Carga de archivo de configuracion
	datosCPU.configuracionCPU = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU);

	//Creacion archivo de log
	if(LOGS_ACTIVADOS == 1)
	{
		datosCPU.logCPU = log_create(RUTA_DE_ARCHIVO_DE_LOGS_CPU, "CPU", 1, LOG_LEVEL_TRACE);
	}

	//Conexion a Planificador
	datosCPU.socketParaPlanificador = crearSocket();
	conectarAServidor(datosCPU.socketParaPlanificador, datosCPU.configuracionCPU->ipPlanificador, datosCPU.configuracionCPU->puertoPlanificador);


	//Conexion a Memoria
	datosCPU.socketParaMemoria = crearSocket();
	conectarAServidor(datosCPU.socketParaMemoria, datosCPU.configuracionCPU->ipMemoria, datosCPU.configuracionCPU->puertoMemoria);

	//Crea tantos "CPUs" (hilos), especificado en el archivo de configuracion
	pthread_t* hiloCPU[datosCPU.configuracionCPU->cantidadDeHilos];
	int i;
	for(i = 0; i < datosCPU.configuracionCPU->cantidadDeHilos; i++)
	{
		datosCPU.idCPU = i;
		pthread_create(&hiloCPU[i], NULL, unCPU, &datosCPU);
		pthread_join(hiloCPU[i], NULL);
	}

	liberarSocket(datosCPU.socketParaMemoria);
	liberarSocket(datosCPU.socketParaPlanificador);
	return 0;
}


//Hilo CPU
void* unCPU(t_hiloCPU* datosCPU)
{
	if(DEBUG == 1)
	{
		printf("idCPU: %i | CPU INICIADA\n", datosCPU->idCPU);
	}

	//Primer mensaje para planificador, diciendo que estoy online y me responde con el quantum, si quantum = 0 -> FIFO
	enviarMensaje(datosCPU->socketParaPlanificador, &datosCPU->idCPU, sizeof(datosCPU->idCPU));

	int quantum = 0;
	recibirMensajeCompleto(datosCPU->socketParaPlanificador, &quantum, sizeof(int));

	if(DEBUG == 1)
	{
		printf("idCPU: %i | QUANTUM RECIBIDO (0 = FIFO): %i\n", datosCPU->idCPU, quantum);
	}

	//LOG: CPU creada conectada/conectada
	if(LOGS_ACTIVADOS == 1)
	{
		log_trace(datosCPU->logCPU, "CPU ID: %i CREADA/CONECTADA A MEMORIA\n", datosCPU->idCPU);
	}

	//Espero a recibir tarea del planificador
	while(true)
	{
		tipoPCB* PCB = recibirPCB(datosCPU->socketParaPlanificador);

		if(DEBUG == 1)
		{
			printf("idCPU: %i | PCB RECIBIDO: ", datosCPU->idCPU);
			imprimirPCB(PCB);
		}

		//LOG: CPU recibe PCBs
		if(LOGS_ACTIVADOS == 1)
		{
			log_trace(datosCPU->logCPU, "CPU ID: %i | PCB RECIBIDO | RUTA: %s | ESTADO: %c | PID: %i | INSPOINTER: %i | QUANTUM (0=FIFO): %i\n", datosCPU->idCPU, PCB->ruta, PCB->estado, PCB->pid, PCB->insPointer, quantum);
		}

		ejecutarPrograma(PCB, quantum, datosCPU);
	}

	return 0;
}
