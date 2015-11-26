#include "funcionesCPU.h"

int idCPUAAsignar = 1;

int main(void)
{
	//Inicializo semaforo para esperar a que se cree el socket master para planificador antes de que se comiencen a crear hilos de CPU
	sem_init(&semaforoConexionMasterPlanificador, 0, 0);

	//Inicializo semaforo para asignar correctamente idCPU a cada hilo
	sem_init(&semaforoHiloCPU, 0, 1);

	//Inicializo semaforo para logs
	sem_init(&semaforoLogs, 0, 1);

	//Inicializo semaforo para el contador de instrucciones de cada CPU
	sem_init(&semaforoInstruccionesCPU, 0, 1);

	//Inicializo semaforo que avisa cuando CPU comienza a trabajar
	sem_init(&semaforoCPUTrabajando, 0, 0);

	//Inicializo semaforo para variable compartida
	sem_init(&semaforoContadorDeInstrucciones, 0, 1);


	//Declaro estructura que requiere un hilo de CPU (configuracion y logs)
	t_hiloCPU hilosCPU;

	//Creo el contador de instrucciones ejecutados por cada CPU
	cantidadDeInstruccionesEjecutadasPorCPUs = list_create();

	//Carga de archivo de configuracion
	hilosCPU.configuracionCPU = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU);

	//Creacion archivo de log
	if(LOGS_ACTIVADOS == 1)
	{
		hilosCPU.logCPU = log_create(RUTA_DE_ARCHIVO_DE_LOGS_CPU, "CPU", 0, LOG_LEVEL_TRACE);
	}
	
	//Inicializo contador de instrucciones ejecutados por cada CPU
	asignarCantidadDeCPUsALista(hilosCPU.configuracionCPU);

	//Creo hilo de conexion master Planificador y continuo con el programa
	pthread_t hiloConexionMasterPlanificador;
	pthread_create(&hiloConexionMasterPlanificador, NULL, (void*)conexionMasterPlanificador, hilosCPU.configuracionCPU);
	sem_wait(&semaforoConexionMasterPlanificador);

	//Creo hilo que reinicia el contador de instrucciones cada 1 minuto
	pthread_t hiloResetearInstruccionesDeCPUs;
	pthread_create(&hiloResetearInstruccionesDeCPUs, NULL, (void*)resetearInstruccionesDeCPUs, hilosCPU.configuracionCPU);

	//Crea tantos "CPUs" (hilos), especificado en el archivo de configuracion
	pthread_t hiloCPU[hilosCPU.configuracionCPU->cantidadDeHilos];

	int i;
	for(i = 0; i < hilosCPU.configuracionCPU->cantidadDeHilos; i++)
	{
		sem_wait(&semaforoHiloCPU);
		pthread_create(&hiloCPU[i], NULL, (void*)unCPU, &hilosCPU);
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
	//Parametros para crear un hilo de CPU: idCPU, archivoDeConfiguracion, archivoDeLogs, socketParaPlanificador, socketParaMemoria
	t_datosCPU datosCPU;
	

	datosCPU.idCPU = idCPUAAsignar;
	idCPUAAsignar++;


	datosCPU.configuracionCPU = hiloCPU->configuracionCPU;
	datosCPU.logCPU = hiloCPU->logCPU;

	if(DEBUG == 1)
	{
		printf("idCPU: %i | HILO CPU CREADO\n", datosCPU.idCPU);
	}

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
		sem_wait(&semaforoLogs);
		log_trace(datosCPU.logCPU, "CPU ID: %i CREADA Y CONECTADA A MEMORIA", datosCPU.idCPU);
		sem_post(&semaforoLogs);
	}

	sem_post(&semaforoHiloCPU);

	//Espero a recibir tarea del planificador
	while(true)
	{
		tipoPCB* PCB = recibirPCB(datosCPU.socketParaPlanificador);

		sem_post(&semaforoCPUTrabajando);

		if(DEBUG == 1)
		{
			printf("idCPU: %i | PCB RECIBIDO: ", datosCPU.idCPU);
			imprimirPCB(PCB);
		}

		//LOG: CPU recibe PCB
		if(LOGS_ACTIVADOS == 1)
		{
			sem_wait(&semaforoLogs);
			if(quantum == 0)
			{
				log_trace(datosCPU.logCPU, "CPU ID: %i | PCB RECIBIDO | RUTA: %s | ESTADO: %c | PID: %i | INSPOINTER: %i | PLANIFICACION: FIFO", datosCPU.idCPU, PCB->ruta, PCB->estado, PCB->pid, PCB->insPointer);
			}
			else
			{
				log_trace(datosCPU.logCPU, "CPU ID: %i | PCB RECIBIDO | RUTA: %s | ESTADO: %c | PID: %i | INSPOINTER: %i | PLANIFICACION: ROUNDROBIN | QUANTUM: %i", datosCPU.idCPU, PCB->ruta, PCB->estado, PCB->pid, PCB->insPointer, quantum);
			}
			sem_post(&semaforoLogs);
		}

		//Funcion principal que va a ejecutar el programa
		ejecutarPrograma(PCB, quantum, &datosCPU);
	}

	return 0;
}

//Hilo de conexion Master con Planificador
void* conexionMasterPlanificador(tipoConfigCPU* configuracionCPU)
{
	int socketMasterPlanificador = crearSocket();
	conectarAServidor(socketMasterPlanificador, configuracionCPU->ipPlanificador, configuracionCPU->puertoPlanificador);

	sem_post(&semaforoConexionMasterPlanificador);

	if(DEBUG == 1)
	{
		printf("PROCESO CPU MASTER CONECTADO A PLANIFICADOR\n");
	}

	char instruccionDePlanificador;

	int cantidadDeInstruccionesDeUnProceso = 0;
	size_t tamanioDeRutaDePrograma;
	char* rutaDelPrograma;

	int socketParaMemoria = crearSocket();
	conectarAServidor(socketParaMemoria, configuracionCPU->ipMemoria, configuracionCPU->puertoMemoria);
	tipoInstruccion instruccionAMemoria;

	while(true)
	{
		recibirMensajeCompleto(socketMasterPlanificador, &instruccionDePlanificador, sizeof(char));
		if(instruccionDePlanificador == CANTIDAD_DE_INSTRUCCIONES_DE_UN_PROCESO)
		{
			recibirMensajeCompleto(socketMasterPlanificador, &tamanioDeRutaDePrograma, sizeof(size_t));
			rutaDelPrograma = malloc(tamanioDeRutaDePrograma);
			recibirMensajeCompleto(socketMasterPlanificador, rutaDelPrograma, tamanioDeRutaDePrograma);

			if(DEBUG == 1)
			{
				printf("CANTIDAD DE INSTRUCCIONES DEL PROCESO: %s SOLICITADO POR PLANIFICADOR\n", rutaDelPrograma);
			}

			cantidadDeInstruccionesDeUnProceso = cantidadDeInstrucciones(rutaDelPrograma);
			enviarMensaje(socketMasterPlanificador, &cantidadDeInstruccionesDeUnProceso, sizeof(cantidadDeInstruccionesDeUnProceso));
			free(rutaDelPrograma);
			if(DEBUG == 1)
			{
				printf("CANTIDAD DE INSTRUCCIONES DEL PROCESO: %s (CANTIDAD: %i) ENVIADO A PLANIFICADOR\n", rutaDelPrograma, cantidadDeInstruccionesDeUnProceso);
			}
		}

		if(instruccionDePlanificador == PORCENTAJE_DE_USO_DEL_CPU)
		{
			if(DEBUG == 1)
			{
				printf("PORCENTAJE DE USO DEL CPU SOLICITADO POR PLANIFICADOR\n");
			}
			enviarPorcentajeDeUso(socketMasterPlanificador, configuracionCPU);
		}

		if(instruccionDePlanificador == FINALIZAR_CPU)
		{
			if(DEBUG == 1)
			{
				printf("PROCESO PLANIFICADOR FINALIZA\n");
			}
			instruccionAMemoria.pid = 0;
			instruccionAMemoria.instruccion = FINALIZAR_PROCESO;
			instruccionAMemoria.nroPagina = 0;
			instruccionAMemoria.texto = string_duplicate("(null)");
			enviarInstruccion(socketParaMemoria, &instruccionAMemoria);

			if(DEBUG == 1)
			{
				printf("INSTRUCCION ENVIADA A MEMORIA | pID: %i | instruccion: %c | numeroDePagina: %i | texto: %s\n", instruccionAMemoria.pid, instruccionAMemoria.instruccion, instruccionAMemoria.nroPagina, instruccionAMemoria.texto);
			}
			
			list_destroy(cantidadDeInstruccionesEjecutadasPorCPUs);
			sem_destroy(&semaforoConexionMasterPlanificador);
			sem_destroy(&semaforoHiloCPU);
			sem_destroy(&semaforoLogs);
			sem_destroy(&semaforoInstruccionesCPU);
			sem_destroy(&semaforoCPUTrabajando);
			sem_destroy(&semaforoContadorDeInstrucciones);
			liberarSocket(socketMasterPlanificador);
			liberarSocket(socketParaMemoria);
			abort();
		}
	}
}


//Hilo que reinicia el contador de instrucciones a 0 cada un minuto
void* resetearInstruccionesDeCPUs(tipoConfigCPU* configuracionCPU)
{
	sem_wait(&semaforoCPUTrabajando);
	while(true)
	{
		sleep(60);
		reiniciarCantidadDeInstrucciones(configuracionCPU);
	}
}

