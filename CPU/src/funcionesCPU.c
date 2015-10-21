#include "funcionesCPU.h"


tipoConfigCPU* crearConfigCPU()
{
	tipoConfigCPU* cfg = malloc(sizeof(tipoConfigCPU));
	cfg->ipPlanificador = string_new();
	cfg->ipMemoria = string_new();

	return cfg;
}

void destruirConfigCPU(tipoConfigCPU* cfg)
{
	free(cfg->ipPlanificador);
	free(cfg->ipMemoria);
	free(cfg);
}

tipoConfigCPU* cargarArchivoDeConfiguracionDeCPU(char* rutaDelArchivoDeConfiguracionDelCPU)
{
	t_config* archivoCfg = config_create(rutaDelArchivoDeConfiguracionDelCPU);
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

int ejecutarPrograma(tipoPCB *PCB, int quantum, int tiempoDeRetardo, int socketParaPlanificador, int socketParaMemoria)
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
			tipoDeSalida = ejecutarInstruccion(instrucciones[PCB->insPointer], PCB->pid, socketParaPlanificador, socketParaMemoria);
			sleep(tiempoDeRetardo);
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
			tipoDeSalida = ejecutarInstruccion(instrucciones[PCB->insPointer], PCB->pid, socketParaPlanificador, socketParaMemoria);
			sleep(tiempoDeRetardo);
			instructionPointer++;
			clock++;
			if(tipoDeSalida == 1)
			{
				break;
			}
		}
		char tipoSalidaParaPlanificador = 'Q';
		enviarMensaje(socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(tipoSalidaParaPlanificador));
	}

	//Salida a Planificador del PCB actualizado
	tipoPCB PCBRespuesta;
	PCBRespuesta = *PCB;
	PCBRespuesta.insPointer = instructionPointer;
	enviarPCB(socketParaPlanificador, PCBRespuesta);
	fclose(programa);
	return tipoDeSalida;
}

//Lector de codigo
int ejecutarInstruccion(char* instruccion, int idDeProceso, int socketParaPlanificador, int socketParaMemoria)
{
	enum estadoDeLectura {INICIAL, EN_VALOR, EN_ORACION} estadoActual = INICIAL;
	int contadorDesdePalabra = 0;
	int contadorHastaPalabra = 0;
	char* nombreDeInstruccion;
	char* valorDeInstruccion;
	char* valorDeInstruccion2;

	int i;
	for(i = 0; i < string_length(instruccion); i++)
	{
		switch(estadoActual)
		{
		case INICIAL:
			if(instruccion[i] == ' ' || instruccion[i] == ';')
			{
				nombreDeInstruccion = string_substring_until(instruccion, contadorHastaPalabra);
				contadorHastaPalabra = 0;
				if(!esInstruccionFinalizar(nombreDeInstruccion))
				{
					contadorDesdePalabra = string_length(nombreDeInstruccion) + 1;
					estadoActual = EN_VALOR;
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
				valorDeInstruccion = string_substring(instruccion, contadorDesdePalabra, contadorHastaPalabra);
				contadorHastaPalabra = 0;
				if(esInstruccionEscribir(nombreDeInstruccion))
				{
					contadorDesdePalabra = string_length(nombreDeInstruccion) + 1 + string_length(valorDeInstruccion) + 1;
					estadoActual = EN_ORACION;
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
				valorDeInstruccion2 = string_substring(instruccion, contadorDesdePalabra, contadorHastaPalabra);
				contadorHastaPalabra = 0;
			}
			else
			{
				contadorHastaPalabra++;
			}
			break;
		}
	}
	int instruccionBloqueante = 0;
	if(esInstruccionIniciar(nombreDeInstruccion))
	{
		instruccionBloqueante = instruccionIniciar(atoi(valorDeInstruccion), idDeProceso, socketParaPlanificador, socketParaMemoria);
	}
	if(esInstruccionLeer(nombreDeInstruccion))
	{
		instruccionBloqueante = instruccionLeer(atoi(valorDeInstruccion), idDeProceso, socketParaPlanificador, socketParaMemoria);
	}
	if(esInstruccionEscribir(nombreDeInstruccion))
	{
		instruccionBloqueante = instruccionEscribir(atoi(valorDeInstruccion), valorDeInstruccion2, idDeProceso, socketParaPlanificador,socketParaMemoria);
	}
	if(esInstruccionEntradaSalida(nombreDeInstruccion))
	{
		instruccionBloqueante = instruccionEntradaSalida(atoi(valorDeInstruccion), idDeProceso, socketParaPlanificador);
	}
	if(esInstruccionFinalizar(nombreDeInstruccion))
	{
		instruccionBloqueante = instruccionFinalizar(idDeProceso, socketParaPlanificador, socketParaMemoria);
	}
	free(nombreDeInstruccion);
	return instruccionBloqueante;
}

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


char* sacarComillas(char* frase)
{
	if(string_starts_with(frase, "\"") && string_ends_with(frase, "\""))
			return string_substring_until(string_substring_from(frase, 1), string_length(frase) -1);
		else
			return frase;
}

//Funcion Iniciar CantidadDePaginas;
int instruccionIniciar(int cantidadDePaginas, int idDeProceso, int socketParaPlanificador,int socketParaMemoria)
{
	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, 'i', cantidadDePaginas, "", socketParaMemoria);

	if(respuestaDeMemoria->respuesta == 'M') //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(char));
	}

	//LOGEAR
	printf("Se ejecuto la instruccion INICIAR Pagina: %i, pID: %i\n", cantidadDePaginas, idDeProceso);
	return 0;
}

//Funcion Leer NumeroDePagina;
int instruccionLeer(int numeroDePagina, int idDeProceso, int socketParaPlanificador, int socketParaMemoria)
{
	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, 'l', numeroDePagina, "", socketParaMemoria);

	if(respuestaDeMemoria->respuesta == 'M') //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(char));
	}

	printf("\n%s\n", respuestaDeMemoria->informacion);

	//LOGEAR
	printf("Se ejecuto la instruccion LEER Pagina: %i, pID: %i", numeroDePagina, idDeProceso);
	return 0;
}

//Funcion Escribir NumeroDePagina Contenido;
int instruccionEscribir(int numeroDePagina, char* textoAEscribir, int idDeProceso, int socketParaPlanificador,int socketParaMemoria)
{
	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, 'e', numeroDePagina, textoAEscribir, socketParaMemoria);

	if(respuestaDeMemoria->respuesta == 'M') //Si fallo la operacion
	{
		char tipoSalidaParaPlanificador = 'F';
		enviarMensaje(socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(char));
	}

	//LOGEAR
	printf("Se ejecuto la instruccion ESCRIBIR Pagina: %i, Contenido: %s, pID: %i", numeroDePagina, textoAEscribir, idDeProceso);
	return 0;
}

//Funcion Entrada-Salida Tiempo;
int instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso, int socketParaPlanificador)
{
	int* tiempoEspera = tiempoDeEspera;
	char tipoSalidaParaPlanificador = 'B';

	enviarMensaje(socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(char));
	enviarMensaje(socketParaPlanificador, &tiempoEspera, sizeof(int));

	//LOGEAR
	printf("Se ejecuto la instruccion ENTRADA-SALIDA Tiempo de espera: %i segundos, pID: %i", tiempoDeEspera, idDeProceso);
	return 1; //Devuelve 1, operacion bloqueante
}

//Funcion Finalizar;
int instruccionFinalizar(int idDeProceso, int socketParaPlanificador, int socketParaMemoria)
{
	tipoRespuesta* respuestaDeMemoria = enviarInstruccionAMemoria(idDeProceso, 'f', 0, "", socketParaMemoria);

	char tipoSalidaParaPlanificador = 'F';
	enviarMensaje(socketParaPlanificador, &tipoSalidaParaPlanificador, sizeof(char));

	//LOGEAR
	printf("Se ejecuto la instruccion FINALIZAR, pID: %i", idDeProceso);
	return 1; //Devuelve 1, operacion bloqueante
}

//Envio de Instruccion a Memoria. Recepcion de Respuesta de Memoria
tipoRespuesta* enviarInstruccionAMemoria(int idDeProceso, char instruccion, int numeroDePagina, char* texto, int socketParaMemoria)
{
	tipoInstruccion instruccionAMemoria;
	instruccionAMemoria.pid = idDeProceso;
	instruccionAMemoria.instruccion = instruccion;
	instruccionAMemoria.nroPagina = numeroDePagina;
	instruccionAMemoria.texto = texto;
	enviarInstruccion(socketParaMemoria, instruccionAMemoria);
	return recibirRespuesta(socketParaMemoria);
}



