#include "funcionesCPU.h"

int main(void)
{
	//Inicializo semaforo para esperar a que se cree el socket master para planificador antes de que se comiencen a crear hilos de CPU
	sem_init(&semaforoConexionMasterPlanificador, 0, 0);

	//Inicializo semaforo para logs
	sem_init(&semaforoLogs, 0, 1);

	//Inicializo semaforo para el contador de instrucciones de cada CPU
	sem_init(&semaforoInstruccionesCPU, 0, 1);

	//Inicializo semaforo que avisa cuando CPU comienza a trabajar
	sem_init(&semaforoCPUTrabajando, 0, 0);

	//Inicializo semaforo para variable compartida
	sem_init(&semaforoContadorDeInstrucciones, 0, 1);

	//Creo el contador de instrucciones ejecutados por cada CPU
	cantidadDeInstruccionesEjecutadasPorCPUs = list_create();

	//Carga de archivo de configuracion
	tipoConfigCPU* configuracionCPU = cargarArchivoDeConfiguracionDeCPU(RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU);

	//Imprime en pantalla informacion importante
	printf(BLANCO "Proceso CPU " VERDE "Iniciado" BLANCO ".\n" FINDETEXTO);
	printf(BLANCO "Cantidad de Hilos: " AZUL "%i" BLANCO ".\n" FINDETEXTO, configuracionCPU->cantidadDeHilos);
	printf(AMARILLO "Esperando procesos Planificador y Administrador de Memoria.\n\n" FINDETEXTO);

	//Creacion archivo de log
	t_log* logCPU = log_create(RUTA_DE_ARCHIVO_DE_LOGS_CPU, "CPU", 0, LOG_LEVEL_TRACE);
	
	//Inicializo contador de instrucciones ejecutados por cada CPU
	asignarCantidadDeCPUsALista(configuracionCPU->cantidadDeHilos);

	//Creo hilo de conexion master Planificador y continuo con el programa
	pthread_t hiloConexionMasterPlanificador;
	pthread_create(&hiloConexionMasterPlanificador, NULL, (void*)conexionMasterPlanificador, configuracionCPU);
	sem_wait(&semaforoConexionMasterPlanificador);

	//Creo hilo que reinicia el contador de instrucciones cada 1 minuto
	pthread_t hiloResetearInstruccionesDeCPUs;
	pthread_create(&hiloResetearInstruccionesDeCPUs, NULL, (void*)resetearInstruccionesDeCPUs, &(configuracionCPU->cantidadDeHilos));

	//Crea tantos "CPUs" (hilos), especificado en el archivo de configuracion
	pthread_t hiloCPU;

	int i;
	for(i = 1; i <= configuracionCPU->cantidadDeHilos; i++)
	{
		//Declaro estructura que requiere un hilo de CPU (configuracion y logs)
		t_datosCPU* datosCPU = malloc(sizeof(t_datosCPU));
		(*datosCPU).idCPU = i;
		datosCPU->configuracionCPU = configuracionCPU;
		datosCPU->logCPU = logCPU;

		if(DEBUG == 1)
		{
			printf("idCPU: %i | HILO CPU CREADO\n", datosCPU->idCPU);
		}

		datosCPU->socketParaPlanificador = crearSocket();
		conectarAServidor(datosCPU->socketParaPlanificador, datosCPU->configuracionCPU->ipPlanificador, datosCPU->configuracionCPU->puertoPlanificador);

		if(DEBUG == 1)
		{
			printf("idCPU: %i | CPU CONECTADA A PLANIFICADOR\n", datosCPU->idCPU);
		}

		datosCPU->socketParaMemoria = crearSocket();
		conectarAServidor(datosCPU->socketParaMemoria, datosCPU->configuracionCPU->ipMemoria, datosCPU->configuracionCPU->puertoMemoria);

		if(DEBUG == 1)
		{
			printf("idCPU: %i | CPU CONECTADA A MEMORIA\n", datosCPU->idCPU);
		}

		//Primer mensaje para planificador, diciendo que estoy online y me responde con el quantum, si quantum = 0 -> FIFO
		enviarMensaje(datosCPU->socketParaPlanificador, &(datosCPU->idCPU), sizeof(datosCPU->idCPU));

		if(DEBUG == 1)
		{
			printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | idCPU: %i\n", datosCPU->idCPU, datosCPU->idCPU);
		}

		recibirMensajeCompleto(datosCPU->socketParaPlanificador, &(datosCPU->quantum), sizeof(int));

		if(DEBUG == 1)
		{
			printf("idCPU: %i | QUANTUM RECIBIDO (0 = FIFO): %i\n", datosCPU->idCPU, datosCPU->quantum);
		}

		printf(BLANCO "CPU id: " AZUL "%i " VERDE "Iniciado" BLANCO ".\n" FINDETEXTO, datosCPU->idCPU);

		sem_wait(&semaforoLogs);
		log_trace(datosCPU->logCPU, "CPU ID: %i CREADA Y CONECTADA A MEMORIA", datosCPU->idCPU);
		sem_post(&semaforoLogs);

		pthread_create(&hiloCPU, NULL, (void*)unCPU, datosCPU);
	}

	for(i = 0; i < configuracionCPU->cantidadDeHilos; i++)
	{
		pthread_join(hiloCPU, NULL);
	}

	return 0;
}


//Hilo CPU
void* unCPU(t_datosCPU* datosCPU)
{
	while(true)
	{
		tipoPCB* PCB = recibirPCB(datosCPU->socketParaPlanificador);

		sem_post(&semaforoCPUTrabajando);

		if(DEBUG == 1)
		{
			printf("idCPU: %i | PCB RECIBIDO: ", datosCPU->idCPU);
			imprimirPCB(PCB);
		}

		if(datosCPU->quantum == 0)
		{
			sem_wait(&semaforoLogs);
			log_trace(datosCPU->logCPU, "CPU ID: %i | PCB RECIBIDO | RUTA: %s | ESTADO: %c | PID: %i | INSPOINTER: %i | PLANIFICACION: FIFO", datosCPU->idCPU, PCB->ruta, PCB->estado, PCB->pid, PCB->insPointer);
			sem_post(&semaforoLogs);
		}
		else
		{
			sem_wait(&semaforoLogs);
			log_trace(datosCPU->logCPU, "CPU ID: %i | PCB RECIBIDO | RUTA: %s | ESTADO: %c | PID: %i | INSPOINTER: %i | PLANIFICACION: ROUNDROBIN | QUANTUM: %i", datosCPU->idCPU, PCB->ruta, PCB->estado, PCB->pid, PCB->insPointer, datosCPU->quantum);
			sem_post(&semaforoLogs);
		}

		//Funcion principal que va a ejecutar el programa
		ejecutarPrograma(PCB, datosCPU);
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

	char instruccionDePlanificador = '0';

	int cantidadDeInstruccionesDeUnProceso = 0;
	size_t tamanioDeRutaDePrograma;
	char* rutaDelPrograma;

	int socketParaMemoria = crearSocket();
	conectarAServidor(socketParaMemoria, configuracionCPU->ipMemoria, configuracionCPU->puertoMemoria);
	tipoInstruccion instruccionAMemoria;

	char mensajeDeValidacionDeArchivo;

	while(instruccionDePlanificador != FINALIZAR_PROCESO)
	{
		recibirMensajeCompleto(socketMasterPlanificador, &instruccionDePlanificador, sizeof(char));

		if(instruccionDePlanificador == EXISTENCIA_DE_ARCHIVO)
		{
			recibirMensajeCompleto(socketMasterPlanificador, &tamanioDeRutaDePrograma, sizeof(size_t));
			rutaDelPrograma = malloc(tamanioDeRutaDePrograma);
			recibirMensajeCompleto(socketMasterPlanificador, rutaDelPrograma, tamanioDeRutaDePrograma);

			if(DEBUG == 1)
			{
				printf("VALIDAR EXISTENCIA DEL ARCHIVO: %s SOLICITADO POR PLANIFICADOR\n", rutaDelPrograma);
			}

			if(validarExistenciaDeArchivo(rutaDelPrograma) == 1)
			{
				mensajeDeValidacionDeArchivo = 'O';
			}
			else
			{
				mensajeDeValidacionDeArchivo = 'E';
			}

			enviarMensaje(socketMasterPlanificador, &mensajeDeValidacionDeArchivo, sizeof(mensajeDeValidacionDeArchivo));

			if(DEBUG == 1)
			{
				printf("VALIDAR EXISTENCIA DEL ARCHIVO: %s. RESPUESTA ENVIADA A PLANIFICADOR: %c\n", rutaDelPrograma, mensajeDeValidacionDeArchivo);
			}
			free(rutaDelPrograma);
		}
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
				printf("MENSAJE RECIBIDO DE PLANIFICADOR: FINALIZAR TODOS LOS PROCESOS\n");
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
			
			tipoRespuesta* respuestaDeMemoria = recibirRespuesta(socketParaMemoria);

			if(respuestaDeMemoria->respuesta == PERFECTO)
			{
				enviarRespuesta(socketMasterPlanificador, respuestaDeMemoria);
				destruirTipoRespuesta(respuestaDeMemoria);
				list_destroy(cantidadDeInstruccionesEjecutadasPorCPUs);
				sem_destroy(&semaforoConexionMasterPlanificador);
				sem_destroy(&semaforoLogs);
				sem_destroy(&semaforoInstruccionesCPU);
				sem_destroy(&semaforoCPUTrabajando);
				sem_destroy(&semaforoContadorDeInstrucciones);
				liberarSocket(socketMasterPlanificador);
				liberarSocket(socketParaMemoria);
				printf(BLANCO "\nProceso CPU " ROJO "Finalizado" BLANCO ".\n" FINDETEXTO);
				exit(EXIT_SUCCESS);
			}
		}
	}
	return 0;
}


//Hilo que reinicia el contador de instrucciones a 0 cada un minuto
void* resetearInstruccionesDeCPUs(int* cantidadDeCPUs)
{
	sleep(10);
	sem_wait(&semaforoCPUTrabajando);
	while(true)
	{
		sleep(60);
		reiniciarCantidadDeInstrucciones(*cantidadDeCPUs);
	}
	return 0;
}

