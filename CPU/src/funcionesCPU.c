#include "funcionesCPU.h"


//Crear archivo de configuracion
tipoConfigCPU* crearConfigCPU()
{
	tipoConfigCPU* cfg = malloc(sizeof(tipoConfigCPU));
	cfg->ipPlanificador = string_new();
	cfg->ipMemoria = string_new();

	return cfg;
}


//Liberar memoria de archivo de configuracion
void destruirConfigCPU(tipoConfigCPU* cfg)
{
	free(cfg->ipPlanificador);
	free(cfg->ipMemoria);
	free(cfg);
}


//Cargar archivo de configuracion
tipoConfigCPU* cargarArchivoDeConfiguracionDeCPU(char* rutaDelArchivoDeConfiguracionDelCPU)
{
	t_config* archivoCfg = config_create(rutaDelArchivoDeConfiguracionDelCPU);
	validarExistenciaDeArchivoDeConfiguracion(rutaDelArchivoDeConfiguracionDelCPU);

	tipoConfigCPU* cfg = crearConfigCPU();

	validarErrorYAbortar(config_has_property(archivoCfg,IP_PLANIFICADOR)
			&& config_has_property(archivoCfg,PUERTO_PLANIFICADOR)
			&& config_has_property(archivoCfg,IP_MEMORIA)
			&& config_has_property(archivoCfg,PUERTO_MEMORIA)
			&& config_has_property(archivoCfg,CANTIDAD_HILOS)
			&& config_has_property(archivoCfg,RETARDO),
			"Las claves del archivo de configuracion no coinciden con las que requiere el CPU");


	cfg->ipPlanificador = string_duplicate(config_get_string_value(archivoCfg,IP_PLANIFICADOR));
	cfg->puertoPlanificador = config_get_int_value(archivoCfg,PUERTO_PLANIFICADOR);
	cfg->ipMemoria = string_duplicate(config_get_string_value(archivoCfg,IP_MEMORIA));
	cfg->puertoMemoria = config_get_int_value(archivoCfg,PUERTO_MEMORIA);
	cfg->cantidadDeHilos = config_get_int_value(archivoCfg,CANTIDAD_HILOS);
	cfg->retardo = config_get_int_value(archivoCfg,RETARDO);

	config_destroy(archivoCfg);

	return cfg;
}


//Cargar archivo a memoria
FILE* abrirProgramaParaLectura(char* rutaDelPrograma)
{
	FILE* programa = fopen(rutaDelPrograma, "r");
	if(programa == NULL)
	{
		perror("El programa no existe o esta vacio.");
		return NULL;
	}
	return programa;
}


//Lector de rafagas
void ejecutarPrograma(tipoPCB *PCB, int quantum, t_datosCPU* datosCPU)
{
	tipoRepuestaDeInstruccion respuestaInstruccion;
	respuestaInstruccion.tipoDeSalida = 0;
	respuestaInstruccion.respuesta = string_new();

	char* respuestasAcumuladas = string_new();

	int instructionPointer = PCB->insPointer;

	FILE* programa = abrirProgramaParaLectura(PCB->ruta);

	char* programaEnMemoria = mmap(0, sizeof(programa), PROT_READ, MAP_SHARED, fileno(programa), 0);
	char** instrucciones = string_split(programaEnMemoria, "\n");

	if(quantum == 0) //FIFO
	{
		while(instructionPointer <= longitudDeStringArray(instrucciones))
		{
			respuestaInstruccion = ejecutarInstruccion(instrucciones[instructionPointer-1], PCB->pid, datosCPU);
			string_append(&respuestasAcumuladas, respuestaInstruccion.respuesta);
			usleep(datosCPU->configuracionCPU->retardo);
			aumentarCantidadDeInstruccionesEjecutadasEnUno(datosCPU->idCPU);
			if(respuestaInstruccion.tipoDeSalida == SALIDA_BLOQUEANTE_POR_ERROR)
			{
				break;
			}


			if(respuestaInstruccion.tipoDeSalida == SALIDA_BLOQUEANTE)
			{
				instructionPointer++;
				break;
			}

			instructionPointer++;
		}
	}

	else //ROUND ROBIN
	{
		int reloj = 0;
		while(reloj < quantum)
		{
			respuestaInstruccion = ejecutarInstruccion(instrucciones[instructionPointer-1], PCB->pid, datosCPU);
			string_append(&respuestasAcumuladas, respuestaInstruccion.respuesta);
			usleep(datosCPU->configuracionCPU->retardo);
			aumentarCantidadDeInstruccionesEjecutadasEnUno(datosCPU->idCPU);
			reloj++;

			if(respuestaInstruccion.tipoDeSalida == SALIDA_BLOQUEANTE_POR_ERROR)
			{
				break;
			}

			if(respuestaInstruccion.tipoDeSalida == SALIDA_BLOQUEANTE)
			{
				instructionPointer++;
				break;
			}

			instructionPointer++;
		}

		if(respuestaInstruccion.tipoDeSalida == CONTINUA_EJECUCION)
		{
			char tipoSalidaParaPlanificador = 'Q';
			enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));

			if(DEBUG == 1)
			{
				printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | pID: %i | mensaje: %c\n", datosCPU->idCPU, PCB->pid, tipoSalidaParaPlanificador);
			}
		}
	}

	//Salida a Planificador del PCB actualizado
	tipoPCB PCBRespuesta;
	PCBRespuesta.ruta = PCB->ruta;
	PCBRespuesta.pid = PCB->pid;
	PCBRespuesta.estado = PCB->estado;
	PCBRespuesta.insPointer = instructionPointer;
	enviarPCB(datosCPU->socketParaPlanificador, &PCBRespuesta);

	if(DEBUG == 1)
	{
		printf("idCPU: %i | PCB ENVIADO A PLANIFICADOR | ", datosCPU->idCPU);
		imprimirPCB(&PCBRespuesta);
	}

	if(LOGS_ACTIVADOS == 1)
	{
		sem_wait(&semaforoLogs);
		log_trace(datosCPU->logCPU, "CPU ID: %i | RAFAGA TERMINADA | PID: %i", datosCPU->idCPU, PCB->pid);
		sem_post(&semaforoLogs);
	}

	//Salida a Planificador de resultados de rafaga
	size_t tamanioDeRespuestasAcumuladas = string_length(respuestasAcumuladas)+sizeof(char);
	enviarMensaje(datosCPU->socketParaPlanificador, &tamanioDeRespuestasAcumuladas, sizeof(size_t));
	enviarMensaje(datosCPU->socketParaPlanificador, respuestasAcumuladas, tamanioDeRespuestasAcumuladas);

	if(DEBUG == 1)
	{
		printf("idCPU: %i | RESPUESTAS DE RAFAGA ENVIADAS A PLANIFICADOR: %s\n", datosCPU->idCPU, respuestasAcumuladas);
	}

	fclose(programa);
}


//Ejecutor de Instrucciones
tipoRepuestaDeInstruccion ejecutarInstruccion(char* lineaDeInstruccion, int idDeProceso, t_datosCPU* datosCPU)
{
	tipoRepuestaDeInstruccion respuestaDeInstruccion;
	respuestaDeInstruccion.tipoDeSalida = 0;
	respuestaDeInstruccion.respuesta = string_new();

	t_instruccion instruccion = extraerInstruccion(lineaDeInstruccion);

	if(esInstruccionIniciar(instruccion.nombreDeInstruccion))
	{
		respuestaDeInstruccion =  instruccionIniciar(atoi(instruccion.valorDeInstruccion1), idDeProceso, datosCPU);
	}
	if(esInstruccionLeer(instruccion.nombreDeInstruccion))
	{
		respuestaDeInstruccion =  instruccionLeer(atoi(instruccion.valorDeInstruccion1), idDeProceso, datosCPU);
	}
	if(esInstruccionEscribir(instruccion.nombreDeInstruccion))
	{
		respuestaDeInstruccion =  instruccionEscribir(atoi(instruccion.valorDeInstruccion1), sacarComillas(instruccion.valorDeInstruccion2), idDeProceso, datosCPU);
	}
	if(esInstruccionEntradaSalida(instruccion.nombreDeInstruccion))
	{
		respuestaDeInstruccion =  instruccionEntradaSalida(atoi(instruccion.valorDeInstruccion1), idDeProceso, datosCPU);
	}
	if(esInstruccionFinalizar(instruccion.nombreDeInstruccion))
	{
		respuestaDeInstruccion =  instruccionFinalizar(idDeProceso, datosCPU);
	}
	return respuestaDeInstruccion;
}


//Lector de codigo
t_instruccion extraerInstruccion(char* instruccion)
{
	t_instruccion instruccionResultado;
	enum estadoDeLectura {INICIAL, EN_VALOR, EN_ORACION} estadoActual = INICIAL;
	int contadorDesdePalabra = 0;
	int contadorHastaPalabra = 0;

	int i;
	for(i = 0; i < string_length(instruccion); i++)
	{
		switch(estadoActual)
		{
		case INICIAL:
			if(instruccion[i] == ' ' || instruccion[i] == ';')
			{
				instruccionResultado.nombreDeInstruccion = string_substring_until(instruccion, contadorHastaPalabra);
				contadorHastaPalabra = 0;
				if(!esInstruccionFinalizar(instruccionResultado.nombreDeInstruccion))
				{
					contadorDesdePalabra = string_length(instruccionResultado.nombreDeInstruccion) + 1;
					estadoActual = EN_VALOR;
				}
				else
				{
					instruccionResultado.valorDeInstruccion1 = "";
					instruccionResultado.valorDeInstruccion2 = "";
				}
			}
			else
			{
				contadorHastaPalabra++;
			}
			break;
		case EN_VALOR:
			if(instruccion[i] == ' ' || instruccion[i] == ';')
			{
				instruccionResultado.valorDeInstruccion1 = string_substring(instruccion, contadorDesdePalabra, contadorHastaPalabra);
				contadorHastaPalabra = 0;
				if(esInstruccionEscribir(instruccionResultado.nombreDeInstruccion))
				{
					contadorDesdePalabra = string_length(instruccionResultado.nombreDeInstruccion) + 1 + string_length(instruccionResultado.valorDeInstruccion1) + 1;
					estadoActual = EN_ORACION;
				}
				else
				{
					instruccionResultado.valorDeInstruccion2 = "";
				}
			}
			else
			{
				contadorHastaPalabra++;
			}
			break;
		case EN_ORACION:
			if(instruccion[i] == ';')
			{
				instruccionResultado.valorDeInstruccion2 = string_substring(instruccion, contadorDesdePalabra, contadorHastaPalabra);
				contadorHastaPalabra = 0;
			}
			else
			{
				contadorHastaPalabra++;
			}
			break;
		}
	}

	return instruccionResultado;
}


//Funcion que dado un array de strings, devuelve la longitud del mismo (cantidad de strings)
int longitudDeStringArray(char** stringArray){
	int i = 0;
	while(stringArray[i])
	{
		i++;
	}
	return i;
}


bool esInstruccionIniciar(char* instruccion)
{
	return string_equals_ignore_case(instruccion, "iniciar");
}

bool esInstruccionLeer(char* instruccion)
{
	return string_equals_ignore_case(instruccion, "leer");
}

bool esInstruccionEscribir(char* instruccion)
{
	return string_equals_ignore_case(instruccion, "escribir");
}

bool esInstruccionEntradaSalida(char* instruccion)
{
	return string_equals_ignore_case(instruccion, "entrada-salida");
}

bool esInstruccionFinalizar(char* instruccion)
{
	return string_equals_ignore_case(instruccion, "finalizar");
}


//Funcion que lee un string y devuelve el string sin las comillas iniciales/finales
char* sacarComillas(char* frase)
{
	if(string_starts_with(frase, "\"") && string_ends_with(frase, "\""))
			return (string_substring_until(string_substring_from(frase, 1), string_length(frase)-2));
		else
			return frase;
}


//Funcion Iniciar CantidadDePaginas;
tipoRepuestaDeInstruccion instruccionIniciar(int cantidadDePaginas, int idDeProceso, t_datosCPU* datosCPU)
{
	tipoRepuestaDeInstruccion respuestaDeInstruccion;
	respuestaDeInstruccion.respuesta = string_new();

	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, INICIAR, cantidadDePaginas, "(null)", datosCPU);

	if(respuestaDeMemoria->respuesta != PERFECTO) //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));

		if(DEBUG == 1)
		{
			printf("idCPU: %i | RESPUESTA DE MEMORIA RECIBIDA | pID: %i | respuesta: %c | informacion: %s\n", datosCPU->idCPU, idDeProceso,respuestaDeMemoria->respuesta, respuestaDeMemoria->informacion);
			printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | pID: %i | mensaje: %c\n", datosCPU->idCPU, idDeProceso, tipoSalidaParaPlanificador);
		}

		if(LOGS_ACTIVADOS == 1)
		{
			sem_wait(&semaforoLogs);
			log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION INICIAR FALLO | PID: %i | CANTIDAD DE PAGINAS: %i | CAUSA: %s", datosCPU->idCPU, idDeProceso, cantidadDePaginas, respuestaDeMemoria->informacion);
			sem_post(&semaforoLogs);
		}

		respuestaDeInstruccion.tipoDeSalida = SALIDA_BLOQUEANTE_POR_ERROR;
		respuestaDeInstruccion.respuesta = string_from_format("mProc %i - Fallo, causa: %s\n", idDeProceso, respuestaDeMemoria->informacion);
		return respuestaDeInstruccion;
	}

	if(DEBUG == 1)
	{
		printf("idCPU: %i | RESPUESTA DE MEMORIA RECIBIDA | pID: %i | respuesta: %c | informacion: %s\n", datosCPU->idCPU, idDeProceso,respuestaDeMemoria->respuesta, respuestaDeMemoria->informacion);
	}

	if(LOGS_ACTIVADOS == 1)
	{
		sem_wait(&semaforoLogs);
		log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION INICIAR EJECUTADA | PID: %i | CANTIDAD DE PAGINAS: %i", datosCPU->idCPU, idDeProceso, cantidadDePaginas);
		sem_post(&semaforoLogs);
	}

	respuestaDeInstruccion.tipoDeSalida = CONTINUA_EJECUCION;
	respuestaDeInstruccion.respuesta = string_from_format("mProc %i - Iniciado\n", idDeProceso);
	return respuestaDeInstruccion;
}


//Funcion Leer NumeroDePagina;
tipoRepuestaDeInstruccion instruccionLeer(int numeroDePagina, int idDeProceso, t_datosCPU* datosCPU)
{
	tipoRepuestaDeInstruccion respuestaDeInstruccion;
	respuestaDeInstruccion.respuesta = string_new();

	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, LEER, numeroDePagina, "(null)", datosCPU);

	if(respuestaDeMemoria->respuesta != PERFECTO) //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));

		if(DEBUG == 1)
		{
			printf("idCPU: %i | RESPUESTA DE MEMORIA RECIBIDA | pID: %i | respuesta: %c | informacion: %s\n", datosCPU->idCPU, idDeProceso, respuestaDeMemoria->respuesta, respuestaDeMemoria->informacion);
			printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | pID: %i | mensaje: %c\n", datosCPU->idCPU, idDeProceso, tipoSalidaParaPlanificador);
		}

		if(LOGS_ACTIVADOS == 1)
		{
			sem_wait(&semaforoLogs);
			log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION LEER FALLO | PID: %i | NUMERO DE PAGINA: %i | CAUSA: %s", datosCPU->idCPU, idDeProceso, numeroDePagina, respuestaDeMemoria->informacion);
			sem_post(&semaforoLogs);
		}

		respuestaDeInstruccion.tipoDeSalida = SALIDA_BLOQUEANTE_POR_ERROR;
		respuestaDeInstruccion.respuesta = string_from_format("mProc %i - Fallo lectura de Pagina %i, causa: %s\n", idDeProceso, numeroDePagina, respuestaDeMemoria->informacion);
		return respuestaDeInstruccion;
	}

	if(DEBUG == 1)
	{
		printf("idCPU: %i | RESPUESTA DE MEMORIA RECIBIDA | pID: %i | respuesta: %c | informacion: %s\n", datosCPU->idCPU, idDeProceso, respuestaDeMemoria->respuesta, respuestaDeMemoria->informacion);
	}

	if(LOGS_ACTIVADOS == 1)
	{
		sem_wait(&semaforoLogs);
		log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION LEER EJECUTADA | PID: %i | NUMERO DE PAGINA: %i | CONTENIDO: %s", datosCPU->idCPU, idDeProceso, numeroDePagina, respuestaDeMemoria->informacion);
		sem_post(&semaforoLogs);
	}

	respuestaDeInstruccion.tipoDeSalida = CONTINUA_EJECUCION;
	respuestaDeInstruccion.respuesta = string_from_format("mProc %i - Pagina %i leida: %s\n", idDeProceso, numeroDePagina, respuestaDeMemoria->informacion);
	return respuestaDeInstruccion;
}


//Funcion Escribir NumeroDePagina Contenido;
tipoRepuestaDeInstruccion instruccionEscribir(int numeroDePagina, char* textoAEscribir, int idDeProceso, t_datosCPU* datosCPU)
{
	tipoRepuestaDeInstruccion respuestaDeInstruccion;
	respuestaDeInstruccion.respuesta = string_new();

	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, ESCRIBIR, numeroDePagina, textoAEscribir, datosCPU);

	if(respuestaDeMemoria->respuesta != PERFECTO) //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));

		if(DEBUG == 1)
		{
			printf("idCPU: %i | RESPUESTA DE MEMORIA RECIBIDA | pID: %i | respuesta: %c | informacion: %s\n", datosCPU->idCPU, idDeProceso, respuestaDeMemoria->respuesta, respuestaDeMemoria->informacion);
			printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | pID: %i | mensaje: %c\n", datosCPU->idCPU, idDeProceso, tipoSalidaParaPlanificador);
		}

		if(LOGS_ACTIVADOS == 1)
		{
			sem_wait(&semaforoLogs);
			log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION ESCRIBIR FALLO | PID: %i | NUMERO DE PAGINA: %i | CAUSA: %s", datosCPU->idCPU, idDeProceso, numeroDePagina, respuestaDeMemoria->informacion);
			sem_post(&semaforoLogs);
		}

		respuestaDeInstruccion.tipoDeSalida = SALIDA_BLOQUEANTE_POR_ERROR;
		respuestaDeInstruccion.respuesta = string_from_format("mProc %i - Fallo escritura de Pagina %i, causa: %s\n", idDeProceso, numeroDePagina, respuestaDeMemoria->informacion);
		return respuestaDeInstruccion;
	}

	if(DEBUG == 1)
	{
		printf("idCPU: %i | RESPUESTA DE MEMORIA RECIBIDA | pID: %i | respuesta: %c | informacion: %s\n", datosCPU->idCPU, idDeProceso, respuestaDeMemoria->respuesta, respuestaDeMemoria->informacion);
	}

	if(LOGS_ACTIVADOS == 1)
	{
		log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION ESCRIBIR EJECUTADA | PID: %i | NUMERO DE PAGINA: %i | CONTENIDO: %s", datosCPU->idCPU, idDeProceso, numeroDePagina, textoAEscribir);
	}

	respuestaDeInstruccion.tipoDeSalida = CONTINUA_EJECUCION;
	respuestaDeInstruccion.respuesta = string_from_format("mProc %i - Pagina %i escrita: %s\n", idDeProceso, numeroDePagina, textoAEscribir);
	return respuestaDeInstruccion;
}


//Funcion Entrada-Salida Tiempo;
tipoRepuestaDeInstruccion instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso, t_datosCPU* datosCPU)
{
	tipoRepuestaDeInstruccion respuestaDeInstruccion;
	respuestaDeInstruccion.respuesta = string_new();

	char tipoSalidaParaPlanificador = 'B';

	enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));
	enviarMensaje(datosCPU->socketParaPlanificador, &tiempoDeEspera, sizeof(tiempoDeEspera));

	if(DEBUG == 1)
	{
		printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | pID: %i | mensaje: %c | tiempoDeEspera: %i\n", datosCPU->idCPU, idDeProceso, tipoSalidaParaPlanificador, tiempoDeEspera);
	}

	if(LOGS_ACTIVADOS == 1)
	{
		sem_wait(&semaforoLogs);
		log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION ENTRADA-SALIDA EJECUTADA | PID: %i | TIEMPO DE ESPERA: %i", datosCPU->idCPU, idDeProceso, tiempoDeEspera);
		sem_post(&semaforoLogs);
	}

	respuestaDeInstruccion.tipoDeSalida = SALIDA_BLOQUEANTE;
	respuestaDeInstruccion.respuesta = string_from_format("mProc %i en entrada-salida de tiempo %i\n", idDeProceso, tiempoDeEspera);
	return respuestaDeInstruccion;
}


//Funcion Finalizar;
tipoRepuestaDeInstruccion instruccionFinalizar(int idDeProceso, t_datosCPU* datosCPU)
{
	tipoRepuestaDeInstruccion respuestaDeInstruccion;
	respuestaDeInstruccion.respuesta = string_new();

	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, FINALIZAR, 0, "(null)", datosCPU);

	if(respuestaDeMemoria->respuesta != PERFECTO) //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));

		if(DEBUG == 1)
		{
			printf("idCPU: %i | RESPUESTA DE MEMORIA RECIBIDA | pID: %i | respuesta: %c | informacion: %s\n", datosCPU->idCPU, idDeProceso, respuestaDeMemoria->respuesta, respuestaDeMemoria->informacion);
			printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | pID: %i | mensaje: %c\n", datosCPU->idCPU, idDeProceso, tipoSalidaParaPlanificador);
		}

		if(LOGS_ACTIVADOS == 1)
		{
			sem_wait(&semaforoLogs);
			log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION FINALIZAR FALLO | PID: %i | CAUSA: %s", datosCPU->idCPU, idDeProceso, respuestaDeMemoria->informacion);
			sem_post(&semaforoLogs);
		}

		respuestaDeInstruccion.tipoDeSalida = SALIDA_BLOQUEANTE_POR_ERROR;
		respuestaDeInstruccion.respuesta = string_from_format("mProc %i no se pudo finalizar, causa: %s\n", idDeProceso, respuestaDeMemoria->informacion);;
		return respuestaDeInstruccion;
	}

	char tipoSalidaParaPlanificador = 'F';
	enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));

	if(DEBUG == 1)
	{
		printf("idCPU: %i | RESPUESTA DE MEMORIA RECIBIDA | pID: %i | respuesta: %c | informacion: %s\n", datosCPU->idCPU, idDeProceso, respuestaDeMemoria->respuesta, respuestaDeMemoria->informacion);
		printf("idCPU: %i | MENSAJE ENVIADO A PLANIFICADOR | pID: %i | mensaje: %c\n", datosCPU->idCPU, idDeProceso, tipoSalidaParaPlanificador);
	}

	if(LOGS_ACTIVADOS == 1)
	{
		sem_wait(&semaforoLogs);
		log_trace(datosCPU->logCPU, "CPU ID: %i | INSTRUCCION FINALIZAR EJECUTADA | PID: %i", datosCPU->idCPU, idDeProceso);
		sem_post(&semaforoLogs);
	}

	respuestaDeInstruccion.tipoDeSalida = SALIDA_BLOQUEANTE;
	respuestaDeInstruccion.respuesta = string_from_format("mProc %i finalizado\n", idDeProceso);
	return respuestaDeInstruccion;
}


//Envio de Instruccion a Memoria. Recepcion de Respuesta de Memoria
tipoRespuesta* enviarInstruccionAMemoria(int idDeProceso, char instruccion, int numeroDePagina, char* texto, t_datosCPU* datosCPU)
{
	tipoInstruccion instruccionAMemoria;
	instruccionAMemoria.pid = idDeProceso;
	instruccionAMemoria.instruccion = instruccion;
	instruccionAMemoria.nroPagina = numeroDePagina;
	instruccionAMemoria.texto = texto;
	enviarInstruccion(datosCPU->socketParaMemoria, &instruccionAMemoria);

	if(DEBUG == 1)
	{
		printf("idCPU: %i | INSTRUCCION ENVIADA A MEMORIA | pID: %i | instruccion: %c | numeroDePagina: %i | texto: %s\n", datosCPU->idCPU, instruccionAMemoria.pid, instruccionAMemoria.instruccion, instruccionAMemoria.nroPagina, instruccionAMemoria.texto);
	}

	return recibirRespuesta(datosCPU->socketParaMemoria);
}


//Instruccion de Planificador que solicita la cantidad de Instrucciones de un Programa
int cantidadDeInstrucciones(char* rutaDelPrograma)
{
	FILE* programa = abrirProgramaParaLectura(rutaDelPrograma);

	char* programaEnMemoria = mmap(0, sizeof(programa), PROT_READ, MAP_SHARED, fileno(programa), 0);
	char** instrucciones = string_split(programaEnMemoria, "\n");
	return longitudDeStringArray(instrucciones);
}


//Inicializa la lista de instrucciones ejecutadas en 0, una por CPU
void asignarCantidadDeCPUsALista(tipoConfigCPU* configuracionCPU)
{
	int i;
	for(i = 0; i < configuracionCPU->cantidadDeHilos; i++)
	{
		int * instruccionesEjecutadas = malloc(sizeof(int));
		*instruccionesEjecutadas = 0;
		list_add(cantidadDeInstruccionesEjecutadasPorCPUs, instruccionesEjecutadas);
	}
}


//Aumenta en 1 el contador de instrucciones ejecutadas por idCPU
void aumentarCantidadDeInstruccionesEjecutadasEnUno(int idCPU)
{
	sem_wait(&semaforoContadorDeInstrucciones);
	int* instruccionesEjecutadas = list_get(cantidadDeInstruccionesEjecutadasPorCPUs, idCPU - 1);
	*instruccionesEjecutadas = *instruccionesEjecutadas + 1;
	sem_post(&semaforoContadorDeInstrucciones);
}


//Reinicia todos los contadores de instrucciones ejecutadas por CPUs
void reiniciarCantidadDeInstrucciones(tipoConfigCPU* configuracionCPU)
{
	int i;
	int* instruccionesEjecutadas;
	for(i = 0; i < configuracionCPU->cantidadDeHilos; i++)
	{
		sem_wait(&semaforoContadorDeInstrucciones);
		instruccionesEjecutadas = list_get(cantidadDeInstruccionesEjecutadasPorCPUs, i);
		*instruccionesEjecutadas = 0;
		sem_post(&semaforoContadorDeInstrucciones);
	}
}


//Envia el porcentaje de uso de cada CPU a planificador
void enviarPorcentajeDeUso(int socketMasterPlanificador, tipoConfigCPU* configuracionCPU)
{
	int i;
	int* instruccionesEjecutadas;
	int porcentajeDeUso;
	for(i = 0; i < configuracionCPU->cantidadDeHilos; i++)
	{
		instruccionesEjecutadas = list_get(cantidadDeInstruccionesEjecutadasPorCPUs, i);
		porcentajeDeUso = *instruccionesEjecutadas * 100 / (configuracionCPU->retardo / 1000000); //retardo en segundos
		enviarMensaje(socketMasterPlanificador, &porcentajeDeUso, sizeof(porcentajeDeUso));
		if(DEBUG == 1)
		{
			printf("PORCENTAJE DE USO DE CPU: %i = %i%% ENVIADO A PLANIFICADOR\n", i + 1, porcentajeDeUso);
		}
	}
}




