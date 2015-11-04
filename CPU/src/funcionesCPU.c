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
int ejecutarPrograma(tipoPCB *PCB, int quantum, t_hiloCPU* datosCPU)
{
	int tipoDeSalida = 0; //Si es 1, corta la ejecución del while que ejecuta instrucciones
	int instructionPointer = PCB->insPointer;
	FILE* programa = abrirProgramaParaLectura(PCB->ruta);
	char* programaEnMemoria = mmap(0, sizeof(programa), PROT_READ, MAP_SHARED, fileno(programa), 0);
	char** instrucciones = string_split(programaEnMemoria, "\n");

	if(quantum == 0) //FIFO
	{
		while(instructionPointer < longitudDeStringArray(instrucciones))
		{
			tipoDeSalida = ejecutarInstruccion(instrucciones[instructionPointer], PCB->pid, datosCPU);
			sleep(datosCPU->configuracionCPU->retardo);
			instructionPointer++;
			if(tipoDeSalida == 1)
			{
				break;
			}
		}
	}

	else //ROUND ROBIN
	{
		int clock = 0;
		while(clock < quantum)
		{
			tipoDeSalida = ejecutarInstruccion(instrucciones[instructionPointer], PCB->pid, datosCPU);
			sleep(datosCPU->configuracionCPU->retardo);
			instructionPointer++;
			clock++;
			if(tipoDeSalida == 1)
			{
				break;
			}
		}
		char tipoSalidaParaPlanificador = 'Q';
		enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));
	}

	//Salida a Planificador del PCB actualizado
	tipoPCB* PCBRespuesta;
	PCBRespuesta->ruta = PCB->ruta;
	PCBRespuesta->pid = PCB->pid;
	PCBRespuesta->estado = PCB->estado;
	PCBRespuesta->insPointer = instructionPointer;
	enviarPCB(datosCPU->socketParaPlanificador, PCBRespuesta);
	fclose(programa);
	return tipoDeSalida;
}

//Ejecutor de Instrucciones
int ejecutarInstruccion(char* lineaDeInstruccion, int idDeProceso, t_hiloCPU* datosCPU)
{
	t_instruccion instruccion = extraerInstruccion(lineaDeInstruccion);
	if(esInstruccionIniciar(instruccion.nombreDeInstruccion))
	{
		return instruccionIniciar(atoi(instruccion.valorDeInstruccion1), idDeProceso, datosCPU);
	}
	if(esInstruccionLeer(instruccion.nombreDeInstruccion))
	{
		return instruccionLeer(atoi(instruccion.valorDeInstruccion1), idDeProceso, datosCPU);
	}
	if(esInstruccionEscribir(instruccion.nombreDeInstruccion))
	{
		return instruccionEscribir(atoi(instruccion.valorDeInstruccion1), instruccion.valorDeInstruccion2, idDeProceso, datosCPU);
	}
	if(esInstruccionEntradaSalida(instruccion.nombreDeInstruccion))
	{
		return instruccionEntradaSalida(atoi(instruccion.valorDeInstruccion1), idDeProceso, datosCPU);
	}
	if(esInstruccionFinalizar(instruccion.nombreDeInstruccion))
	{
		return instruccionFinalizar(idDeProceso, datosCPU);
	}
	return 0;
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
			return string_substring_until(string_substring_from(frase, 1), string_length(frase) -1);
		else
			return frase;
}

//Funcion Iniciar CantidadDePaginas;
int instruccionIniciar(int cantidadDePaginas, int idDeProceso, t_hiloCPU* datosCPU)
{
	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, INICIAR, cantidadDePaginas, "", datosCPU);

	if(respuestaDeMemoria->respuesta == MANQUEADO) //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));
	}

	return 0;
}

//Funcion Leer NumeroDePagina;
int instruccionLeer(int numeroDePagina, int idDeProceso, t_hiloCPU* datosCPU)
{
	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, LEER, numeroDePagina, "", datosCPU);

	if(respuestaDeMemoria->respuesta == MANQUEADO) //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));
	}

	printf("\n%s\n", respuestaDeMemoria->informacion);

	//LOGEAR
	return 0;
}

//Funcion Escribir NumeroDePagina Contenido;
int instruccionEscribir(int numeroDePagina, char* textoAEscribir, int idDeProceso, t_hiloCPU* datosCPU)
{
	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, ESCRIBIR, numeroDePagina, textoAEscribir, datosCPU);

	if(respuestaDeMemoria->respuesta == MANQUEADO) //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));
	}

	//LOGEAR
	return 0;
}

//Funcion Entrada-Salida Tiempo;
int instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso, t_hiloCPU* datosCPU)
{
	char tipoSalidaParaPlanificador = 'B';

	enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));
	enviarMensaje(datosCPU->socketParaPlanificador, &tiempoDeEspera, sizeof(tiempoDeEspera));

	//LOGEAR
	return 1; //Devuelve 1, operacion bloqueante
}

//Funcion Finalizar;
int instruccionFinalizar(int idDeProceso, t_hiloCPU* datosCPU)
{
	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, FINALIZAR, 0, "", datosCPU);

	char tipoSalidaParaPlanificador = 'F';
	enviarMensaje(datosCPU->socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));

	//LOGEAR
	return 1; //Devuelve 1, operacion bloqueante
}

//Envio de Instruccion a Memoria. Recepcion de Respuesta de Memoria
tipoRespuesta* enviarInstruccionAMemoria(int idDeProceso, char instruccion, int numeroDePagina, char* texto, t_hiloCPU* datosCPU)
{
	tipoInstruccion instruccionAMemoria;
	instruccionAMemoria.pid = idDeProceso;
	instruccionAMemoria.instruccion = instruccion;
	instruccionAMemoria.nroPagina = numeroDePagina;
	instruccionAMemoria.texto = texto;
	enviarInstruccion(datosCPU->socketParaMemoria, &instruccionAMemoria);

	if(DEBUG == 1)
	{
		printf("idCPU: %i | INSTRUCCION ENVIADA A MEMORIA: pID: %i | instruccion: %c | numeroDePagina: %i | texto: %s\n", datosCPU->idCPU, instruccionAMemoria.pid, instruccionAMemoria.instruccion, instruccionAMemoria.nroPagina, instruccionAMemoria.texto);
	}

	return recibirRespuesta(datosCPU->socketParaMemoria);
}



