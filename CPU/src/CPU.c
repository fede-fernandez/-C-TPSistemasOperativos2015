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
	printf(BLANCO "Cantidad de Hilos: " AZUL "%i " BLANCO "Retardo: " AMARILLO "%i " BLANCO "microsegundos.\n" FINDETEXTO, configuracionCPU->cantidadDeHilos, configuracionCPU->retardo);
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

	inicioDeProceso = time(0);

	int i;
	for(i = 1; i <= configuracionCPU->cantidadDeHilos; i++)
	{
		//Declaro estructura que requiere un hilo de CPU (configuracion y logs)
		t_datosCPU* datosCPU = malloc(sizeof(t_datosCPU));
		(*datosCPU).idCPU = i;
		datosCPU->configuracionCPU = configuracionCPU;
		datosCPU->logCPU = logCPU;


		datosCPU->socketParaPlanificador = crearSocket();
		conectarAServidor(datosCPU->socketParaPlanificador, datosCPU->configuracionCPU->ipPlanificador, datosCPU->configuracionCPU->puertoPlanificador);

		if(configuracionCPU->debug == 1)
		{
			printf(BLANCO "CPU id: " AZUL "%i " BLANCO "| " VERDE "CONECTADA " BLANCO "A " CELESTE "PLANIFICADOR" BLANCO ".\n" FINDETEXTO, datosCPU->idCPU);
		}

		datosCPU->socketParaMemoria = crearSocket();
		conectarAServidor(datosCPU->socketParaMemoria, datosCPU->configuracionCPU->ipMemoria, datosCPU->configuracionCPU->puertoMemoria);

		if(configuracionCPU->debug == 1)
		{
			printf(BLANCO "CPU id: " AZUL "%i " BLANCO "| " VERDE "CONECTADA " BLANCO "A " CELESTE "MEMORIA" BLANCO ".\n" FINDETEXTO, datosCPU->idCPU);
		}

		//Primer mensaje para planificador, diciendo que estoy online y me responde con el quantum, si quantum = 0 -> FIFO
		enviarMensaje(datosCPU->socketParaPlanificador, &(datosCPU->idCPU), sizeof(datosCPU->idCPU));

		if(configuracionCPU->debug == 1)
		{
			printf(BLANCO "CPU id: " AZUL "%i " BLANCO "| MENSAJE ENVIADO A " CELESTE "PLANIFICADOR " BLANCO "| CONTENIDO: " AMARILLO "%i" BLANCO ".\n" FINDETEXTO, datosCPU->idCPU, datosCPU->idCPU);
		}

		recibirMensajeCompleto(datosCPU->socketParaPlanificador, &(datosCPU->quantum), sizeof(int));


		if(datosCPU->quantum == 0)
		{
			printf(BLANCO "CPU id: " AZUL "%i " VERDE "Iniciado" BLANCO ". Planificacion: " AMARILLO "FIFO" BLANCO ".\n" FINDETEXTO, datosCPU->idCPU);
		}
		else
		{
			printf(BLANCO "CPU id: " AZUL "%i " VERDE "Iniciado" BLANCO ". Planificacion: " AMARILLO "ROUND-ROBIN " BLANCO "(quantum: " AMARILLO "%i" BLANCO ").\n" FINDETEXTO, datosCPU->idCPU, datosCPU->quantum);
		}

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

		if(datosCPU->configuracionCPU->debug == 1)
		{
			printf(BLANCO "CPU id: " AZUL "%i " BLANCO "| PCB RECIBIDO: " FINDETEXTO, datosCPU->idCPU);
			printf(BLANCO "RUTA: " AMARILLO "%s " BLANCO "| PID: " AMARILLO "%i " BLANCO "| INSTRUCTION POINTER: " AMARILLO "%i " BLANCO "| ESTADO: " AMARILLO "%c" BLANCO ".\n" FINDETEXTO, PCB->ruta, PCB->pid, PCB->insPointer, PCB->estado);
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
			log_trace(datosCPU->logCPU, "CPU ID: %i | PCB RECIBIDO | RUTA: %s | ESTADO: %c | PID: %i | INSPOINTER: %i | PLANIFICACION: ROUND-ROBIN | QUANTUM: %i", datosCPU->idCPU, PCB->ruta, PCB->estado, PCB->pid, PCB->insPointer, datosCPU->quantum);
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

	if(configuracionCPU->debug == 1)
	{
		printf(BLANCO "CONEXION MASTER CON " CELESTE "PLANIFICADOR " VERDE "ESTABLECIDA" BLANCO ".\n" FINDETEXTO);
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

			if(configuracionCPU->debug == 1)
			{
				printf(BLANCO "VALIDAR EXISTENCIA DEL ARCHIVO: "  AMARILLO "%s " BLANCO "SOLICITADO POR " CELESTE "PLANIFICADOR" BLANCO ".\n" FINDETEXTO, rutaDelPrograma);
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

			if(configuracionCPU->debug == 1)
			{
				printf(BLANCO "VALIDAR EXISTENCIA DEL ARCHIVO: "  AMARILLO "%s " BLANCO "RESPUESTA ENVIADA A " CELESTE "PLANIFICADOR" BLANCO ":" AMARILLO "%c" BLANCO ".\n" FINDETEXTO, rutaDelPrograma, mensajeDeValidacionDeArchivo);
			}
			free(rutaDelPrograma);
		}
		if(instruccionDePlanificador == CANTIDAD_DE_INSTRUCCIONES_DE_UN_PROCESO)
		{
			recibirMensajeCompleto(socketMasterPlanificador, &tamanioDeRutaDePrograma, sizeof(size_t));
			rutaDelPrograma = malloc(tamanioDeRutaDePrograma);
			recibirMensajeCompleto(socketMasterPlanificador, rutaDelPrograma, tamanioDeRutaDePrograma);

			if(configuracionCPU->debug == 1)
			{
				printf(BLANCO "CANTIDAD DE INSTRUCCIONES DEL PROCESO: " AMARILLO "%s " BLANCO "SOLICITADO POR " CELESTE "PLANIFICADOR" BLANCO ".\n" FINDETEXTO, rutaDelPrograma);
			}

			cantidadDeInstruccionesDeUnProceso = cantidadDeInstrucciones(rutaDelPrograma);
			enviarMensaje(socketMasterPlanificador, &cantidadDeInstruccionesDeUnProceso, sizeof(cantidadDeInstruccionesDeUnProceso));
			free(rutaDelPrograma);
			if(configuracionCPU->debug == 1)
			{
				printf(BLANCO "CANTIDAD DE INSTRUCCIONES DEL PROCESO: " AMARILLO "%s " BLANCO "(CANTIDAD: " AMARILLO "%i" BLANCO ") ENVIADO A " CELESTE "PLANIFICADOR" BLANCO ".\n" FINDETEXTO, rutaDelPrograma, cantidadDeInstruccionesDeUnProceso);
			}
		}

		if(instruccionDePlanificador == PORCENTAJE_DE_USO_DEL_CPU)
		{
			if(configuracionCPU->debug == 1)
			{
				printf(BLANCO "PORCENTAJE DE USO DEL CPU SOLICITADO POR " CELESTE "PLANIFICADOR" BLANCO ".\n" FINDETEXTO);
			}
			enviarPorcentajeDeUso(socketMasterPlanificador, configuracionCPU);
		}

		if(instruccionDePlanificador == FINALIZAR_CPU)
		{
			if(configuracionCPU->debug == 1)
			{
				printf(BLANCO "MENSAJE RECIBIDO DE " CELESTE "PLANIFICADOR" BLANCO ": " ROJO "FINALIZAR TODOS LOS PROCESOS" BLANCO ".\n" FINDETEXTO);
			}
			instruccionAMemoria.pid = 0;
			instruccionAMemoria.instruccion = FINALIZAR_PROCESO;
			instruccionAMemoria.nroPagina = 0;
			instruccionAMemoria.texto = string_duplicate("(null)");
			enviarInstruccion(socketParaMemoria, &instruccionAMemoria);

			if(configuracionCPU->debug == 1)
			{
				printf(BLANCO "INSTRUCCION ENVIADA A " CELESTE "MEMORIA " BLANCO "| PID: " AMARILLO "%i " BLANCO "| INSTRUCCION: " AMARILLO "%c " BLANCO "| NUMERO DE PAGINA: " AMARILLO "%i " BLANCO "| TEXTO: " AMARILLO "%s" BLANCO ".\n" FINDETEXTO, instruccionAMemoria.pid, instruccionAMemoria.instruccion, instruccionAMemoria.nroPagina, instruccionAMemoria.texto);
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
	sem_wait(&semaforoCPUTrabajando);
	while(true)
	{
		sleep(60);
		inicioDeProceso = time(0);
		reiniciarCantidadDeInstrucciones(*cantidadDeCPUs);
	}
	return 0;
}

