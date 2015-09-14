
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

ejecutarInstruccion(char* instruccion, int idDeProceso)
{
	char** instruccionSeparadaPorEspacios = string_split(instruccion, " ");

	if(string_equals_ignore_case(instruccionSeparadaPorEspacios[0], "iniciar"))
		instruccionIniciar(instruccionSeparadaPorEspacios[1], idDeProceso);

	if(string_equals_ignore_case(instruccionSeparadaPorEspacios[0], "leer"))
		instruccionLeer(instruccionSeparadaPorEspacios[1], idDeProceso);

	if(string_equals_ignore_case(instruccionSeparadaPorEspacios[0], "escribir"))
		instruccionEscribir(instruccionSeparadaPorEspacios[1],
				instruccionSeparadaPorEspacios[2], idDeProceso);

	if(string_equals_ignore_case(instruccionSeparadaPorEspacios[0], "entrada-salida"))
		instruccionEntradaSalida(instruccionSeparadaPorEspacios[1], idDeProceso);

	if(string_equals_ignore_case(instruccionSeparadaPorEspacios[0], "finalizar"))
		instruccionFinalizar(idDeProceso);
}

//Por ahora envio a Memoria solo idDelProceso para que lo asigne a la pagina, si necesita
//mas cosas avisenme, o usamos el pcb para todo.

int instruccionIniciar(int numeroDePaginas, int idDeProceso)
{
	//Envio socket a Proceso Memoria con la instrucción iniciar
	//y el número de páginas que voy a pedirle
	//Proceso Memoria me avisa si se pudo hacer o no
	return FALSE;
}

int instruccionLeer(int numeroDePagina)
{
	//Envio socket a Proceso Memoria con la instruccion leer
	//y el numero de pagina que quiero recibir
	//Proceso memoria me devuelve el resultado
	//Imprimo el resultado
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
	return 0;
}

int instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso)
{
	//Envio socket a Proceso Planificador con el pcb, cambiando
	//el estado a BLOQUEADO y el tiempo que voy a estarlo
	//retorno 1 porque tengo que usar break en el while del lector de instrucciones
	return 1;
}

int instruccionFinalizar(int idDeProceso)
{
	//Envio socket a Proceso Memoria para que libere el
	//espacio de memoria ocupado por el proceso, también memoria le avisa a Swap.
	return 1;
}
