#include "funcionesCPU.h"


tipoConfigCPU* crearConfigCPU(){
	tipoConfigCPU* cfg = malloc(sizeof(tipoConfigCPU));
	cfg->ipPlanificador = string_new();
	cfg->ipMemoria = string_new();

	return cfg;
}

void destruirConfigCPU(tipoConfigCPU* cfg){
	free(cfg->ipPlanificador);
	free(cfg->ipMemoria);
	free(cfg);
}

tipoConfigCPU* cargarArchivoDeConfiguracionDeCPU(char* rutaDelArchivoDeConfiguracionDelCPU){

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

int ejecutarPrograma(tipoPCB PCB, int tiempoDeRetardo)
{
	FILE* programa = abrirProgramaParaLectura(PCB.ruta);
	char* programaEnMemoria = mmap(0, sizeof(programa), PROT_READ, MAP_SHARED, fileno(programa), 0);
	char** instrucciones = string_split(programaEnMemoria, "\n");

	while(PCB.insPointer < longitudDeStringArray(instrucciones))
	{
		ejecutarInstruccion(instrucciones[PCB.insPointer], PCB.pid);
		sleep(tiempoDeRetardo);
		PCB.insPointer++;
	}
	return PCB.insPointer;
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

int ejecutarInstruccion(char* instruccion, int idDeProceso)
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
	if(esInstruccionIniciar(nombreDeInstruccion))
	{
		instruccionIniciar(atoi(valorDeInstruccion), idDeProceso);
	}
	if(esInstruccionLeer(nombreDeInstruccion))
	{
		instruccionLeer(atoi(valorDeInstruccion), idDeProceso);
	}
	if(esInstruccionEscribir(nombreDeInstruccion))
	{
		instruccionEscribir(atoi(valorDeInstruccion), valorDeInstruccion2, idDeProceso);
	}
	if(esInstruccionEntradaSalida(nombreDeInstruccion))
	{
		instruccionEntradaSalida(atoi(valorDeInstruccion), idDeProceso);
	}
	if(esInstruccionFinalizar(nombreDeInstruccion))
	{
		instruccionFinalizar(idDeProceso);
	}
	return 0;
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


int instruccionIniciar(int numeroDePaginas, int idDeProceso)
{
	//Envio socket a Proceso Memoria con la instrucción iniciar
	//y el número de páginas que voy a pedirle
	//Proceso Memoria me avisa si se pudo hacer o no
	//Si falla aviso a Planificador
	printf("Se ejecuto la instruccion INICIAR Pagina: %i, pID: %i\n", numeroDePaginas, idDeProceso);
	return 0;
}

int instruccionLeer(int numeroDePagina, int idDeProceso)
{
	//Envio socket a Proceso Memoria con la instrucción leer
	//y el número de página que quiero recibir
	//Proceso memoria me devuelve el contenido de la página
	//Imprimo el contenido
	printf("Se ejecuto la instruccion LEER Pagina: %i, pID: %i", numeroDePagina, idDeProceso);
	return 0;
}

int instruccionEscribir(int numeroDePagina, char* textoAEscribir, int idDeProceso)
{
	//Envio socket a Proceso Memoria con la instruccion escribir,
	//el número de página donde quiero escribir y el texto
	//a escribir, MEMORIA DEBE REEMPLAZAR TODO EL CONTENIDO
	//SI ESA PAGINA TIENE ALGO
	//Proceso Memoria me devuelve el resultado
	//Imprimo el resultado
	printf("Se ejecuto la instruccion ESCRIBIR Pagina: %i, Contenido: %s, pID: %i", numeroDePagina, textoAEscribir, idDeProceso);
	return 0;
}

int instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso)
{
	//Envio socket a Proceso Planificador con el pcb, cambiando
	//el estado a BLOQUEADO y el tiempo que voy a estarlo
	//retorno 1 porque tengo que usar break en el while del lector de instrucciones
	printf("Se ejecuto la instruccion ENTRADA-SALIDA Tiempo de espera: %i segundos, pID: %i", tiempoDeEspera, idDeProceso);
	return 1;
}

int instruccionFinalizar(int idDeProceso)
{
	//Envio socket a Proceso Memoria para que libere el
	//espacio de memoria ocupado por el proceso, también memoria le avisa a Swap.
	printf("Se ejecuto la instruccion FINALIZAR, pID: %i", idDeProceso);
	return 1;
}
