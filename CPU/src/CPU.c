#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"


int main(void)
{
	t_hiloCPU hilosCPU;

	//Carga de archivo de configuracion
	hilosCPU.configuracionCPU = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU);

	//Creacion archivo de log
	if(LOGS_ACTIVADOS == 1)
	{
		hilosCPU.logCPU = log_create(RUTA_DE_ARCHIVO_DE_LOGS_CPU, "CPU", 1, LOG_LEVEL_TRACE);
	}

	//Crea tantos "CPUs" (hilos), especificado en el archivo de configuracion
	pthread_t hiloCPU[hilosCPU.configuracionCPU->cantidadDeHilos];

	int i;
	for(i = 0; i < hilosCPU.configuracionCPU->cantidadDeHilos; i++)
	{
		hilosCPU.idCPU = i;
		pthread_create(&hiloCPU[i], NULL, unCPU, &hilosCPU);
		sleep(1);
	}

	for(i = 0; i < hilosCPU.configuracionCPU->cantidadDeHilos; i++)
	{
		pthread_join(hiloCPU[i], NULL);
	}

	return 0;
}


//Hilo CPU
void* unCPU(t_hiloCPU* hiloCPU)
{
	if(DEBUG == 1)
	{
		printf("idCPU: %i | HILO CPU CREADO\n", hiloCPU->idCPU);
	}

	//Parametros para crear un hiloCPU: idCPU, archivoDeConfiguracion, archivoDeLogs, socketParaPlanificador, socketParaMemoria
	t_datosCPU datosCPU;

	datosCPU.idCPU = hiloCPU->idCPU;
	datosCPU.configuracionCPU = hiloCPU->configuracionCPU;
	datosCPU.logCPU = hiloCPU->logCPU;


	//Conexion a Planificador
	datosCPU.socketParaPlanificador = crearSocket();
	conectarAServidor(datosCPU.socketParaPlanificador, datosCPU.configuracionCPU->ipPlanificador, datosCPU.configuracionCPU->puertoPlanificador);

	if(DEBUG == 1)
	{
		printf("idCPU: %i | CPU CONECTADA A PLANIFICADOR\n", datosCPU.idCPU);
	}

	//Conexion a Memoria
	datosCPU.socketParaMemoria = crearSocket();
	conectarAServidor(datosCPU.socketParaMemoria, datosCPU.configuracionCPU->ipMemoria, datosCPU.configuracionCPU->puertoMemoria);

	if(DEBUG == 1)
	{
		printf("idCPU: %i | CPU CONECTADA A MEMORIA\n", datosCPU.idCPU);
	}

	//Primer mensaje para planificador, diciendo que estoy online y me responde con el quantum, si quantum = 0 -> FIFO
	enviarMensaje(datosCPU.socketParaPlanificador, &datosCPU.idCPU, sizeof(datosCPU.idCPU));

	if(DEBUG == 1)
	{
		printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | idCPU: %i\n", datosCPU.idCPU, datosCPU.idCPU);
	}

	int quantum = 0;
	recibirMensajeCompleto(datosCPU.socketParaPlanificador, &quantum, sizeof(int));

	if(DEBUG == 1)
	{
		printf("idCPU: %i | QUANTUM RECIBIDO (0 = FIFO): %i\n", datosCPU.idCPU, quantum);
	}

	//LOG: CPU creada conectada/conectada
	if(LOGS_ACTIVADOS == 1)
	{
		log_trace(datosCPU.logCPU, "CPU ID: %i CREADA/CONECTADA A MEMORIA\n", datosCPU.idCPU);
	}

	//Espero a recibir tarea del planificador
	while(true)
	{
		tipoPCB* PCB = recibirPCB(datosCPU.socketParaPlanificador);

		if(DEBUG == 1)
		{
			printf("idCPU: %i | PCB RECIBIDO: ", datosCPU.idCPU);
			imprimirPCB(PCB);
		}

		//LOG: CPU recibe PCBs
		if(LOGS_ACTIVADOS == 1)
		{
			log_trace(datosCPU.logCPU, "CPU ID: %i | PCB RECIBIDO | RUTA: %s | ESTADO: %c | PID: %i | INSPOINTER: %i | QUANTUM (0=FIFO): %i\n", datosCPU.idCPU, PCB->ruta, PCB->estado, PCB->pid, PCB->insPointer, quantum);
		}

		ejecutarPrograma(PCB, quantum, &datosCPU);
	}

	liberarSocket(datosCPU.socketParaMemoria);
	liberarSocket(datosCPU.socketParaPlanificador);

	return 0;
}
