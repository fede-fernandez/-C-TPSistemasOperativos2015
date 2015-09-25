/*
 * funcionesMemoria.c
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */
#include "funcionesMemoria.h"


tipoConfigMemoria* crearConfigMemoria(){
	tipoConfigMemoria* cfg = malloc(sizeof(tipoConfigMemoria));
	cfg->ipSWAP = string_new();
	cfg->TLBHabilitada = string_new();

	return cfg;
}

void destruirConfigMemoria(tipoConfigMemoria* estructuraDeConfiguracion){
	free(estructuraDeConfiguracion->ipSWAP);
	free(estructuraDeConfiguracion->TLBHabilitada);
	free(estructuraDeConfiguracion);
}

tipoConfigMemoria* cargarArchivoDeConfiguracionDeMemoria(char* rutaDelArchivoDeConfiguracion){

	t_config* archivoCfg = config_create(rutaDelArchivoDeConfiguracion);
	tipoConfigMemoria* cfg = crearConfigMemoria();

	validarErrorYAbortar(config_has_property(archivoCfg,PUERTO_ESCUCHA)
			&& config_has_property(archivoCfg,IP_SWAP)
			&& config_has_property(archivoCfg,PUERTO_SWAP)
			&& config_has_property(archivoCfg,MAXIMO_MARCOS_POR_PROCESO)
			&& config_has_property(archivoCfg,CANTIDAD_MARCOS)
			&& config_has_property(archivoCfg,TAMANIO_MARCO)
			&& config_has_property(archivoCfg,ENTRADAS_TLB)
			&& config_has_property(archivoCfg,TLB_HABILITADA)
			&& config_has_property(archivoCfg,RETARDO_MEMORIA),
			"Las claves del archivo de configuracion no coinciden con las que requiere el Administrador de Memoria.");

	cfg->puertoDeEscucha = config_get_int_value(archivoCfg,PUERTO_ESCUCHA);
	cfg->ipSWAP = string_duplicate(config_get_string_value(archivoCfg,IP_SWAP));
	cfg->puertoSWAP = config_get_int_value(archivoCfg,PUERTO_SWAP);
	cfg->maximoDeMarcosPorProceso = config_get_int_value(archivoCfg,MAXIMO_MARCOS_POR_PROCESO);
	cfg->cantidadDeMarcos = config_get_int_value(archivoCfg,CANTIDAD_MARCOS);
	cfg->tamanioDeMarco = config_get_int_value(archivoCfg,TAMANIO_MARCO);
	cfg->entradasDeTLB = config_get_int_value(archivoCfg,ENTRADAS_TLB);
	cfg->TLBHabilitada = string_duplicate(config_get_string_value(archivoCfg,TLB_HABILITADA));
	cfg->retardoDeMemoria = config_get_int_value(archivoCfg,RETARDO_MEMORIA);

	config_destroy(archivoCfg);

	return cfg;
}

tipoInstruccion recibirInstruccionCpu(int socketCpu){

	size_t tamanioBloque;

	recibirBloque(&tamanioBloque,socketCpu);

	void* buffer = malloc(tamanioBloque);

	recibirMensajeCompleto(socketCpu,buffer,tamanioBloque);
	
	tipoInstruccion instruccionRecibida = deserializarInstruccion(tamanioBloque, buffer);

	return instruccionRecibida;
}

tipoInstruccion deserializarInstruccion(size_t tamanioBloque,void* buffer){

	tipoInstruccion* instruccion;

	memcpy(buffer,instruccion->instruccion,sizeof(char));tamanioBloque-=sizeof(char);
	memcpy(buffer+sizeof(char),(int*)instruccion->pid,sizeof(int));tamanioBloque-=sizeof(int);
	deserializarIntYCadena((int*)instruccion->nroPagina,instruccion->texto,tamanioBloque,buffer+sizeof(char)+sizeof(int));

	return *instruccion;
}

void* serializarRespuestaCpu(tipoRespuesta respuesta){

	size_t tamanioInformacion = strlen(respuesta.informacion);

	size_t tamanioBloque = tamanioInformacion+sizeof(char);

	void* buffer = malloc(tamanioBloque+sizeof(int));

	memcpy(buffer,&tamanioBloque,sizeof(size_t));
	memcpy(buffer+sizeof(size_t),&respuesta.respuesta,sizeof(char));
	memcpy(buffer+sizeof(size_t)+sizeof(char),&tamanioInformacion,sizeof(size_t));
	memcpy(buffer+2*sizeof(size_t)+sizeof(char),&respuesta.informacion,tamanioInformacion);

	return buffer;
}

void tratarEscritura(int socketParaCpus,int socketParaSwap,int socketParaEscribir){}

void tratarLectura(int socketParaCpus,int socketParaSwap,int socketParaLeer){

	void ejecutarInstrunccion (tipoInstruccion instruccionCPU){
		switch (instruccionCPU.instruccion) {
			case 'i'://inciar
				reservarMemoriaParaProceso(instruccionCPU, socketParaCpus, socketParaSwap);
				break;

			case 'l'://lectura

				break;
			case 'e'://escritura
				break;
			case 's'://entrada y salida
				break;
			case 'f'://finalizar
				break;
		}
	}


}

void tratarLecturas(int socketParaCpus,int socketParaSwap,t_list* listaLectura){

	int var;
	if(!list_is_empty(listaLectura)){
	for (var = 0; var <list_size(listaLectura); ++var) {

		int* socketCpuActual = list_get(listaLectura,var);

		tratarLectura(socketParaCpus,socketParaSwap,*socketCpuActual);
		}
	}
}
void tratarEscrituras(int socketParaCpus,int socketParaSwap,t_list* listaEscritura){

	int var;
	if(!list_is_empty(listaEscritura)){
	for (var = 0; var <list_size(listaEscritura); ++var) {

		int* socketCpuActual = list_get(listaEscritura,var);

		tratarEscritura(socketParaCpus,socketParaSwap,*socketCpuActual);
		}
	}
}

/*************instrucciones*******************/

void ejecutarInstrunccion (tipoInstruccion instruccionCPU){
	switch (instruccionCPU.instruccion) {
		case 'i'://inciar
			break;
		case 'l'://lectura
			break;
		case 'e'://escritura
			break;
		case 's'://entrada y salida
			break;
		case 'f'://finalizar
			break;
	}
}

/****iniciar N*****/
int reservarMemoriaEnSwap(tipoInstruccion instruccion, int socketSwap){
	return 1;
}

int reservarMemoriaEnRam(tipoInstruccion instruccion){
	return 1;
}

void cancelarInicializacion(int procesoID){

}

void reservarMemoriaParaProceso(tipoInstruccion instruccion, int socketCPU, int socketSWAP){
	if (reservarMemoriaEnSwap(instruccion, socketSWAP)) {
		if (reservarMemoriaEnRam(instruccion)) {

		} else {
			//ver algoritmos para pasar procesos de ram a swap y liberar espacio para el nuevo proceso
		}
	} else {
		//cancelarInicializacion(procesoID);
	}

}

/*******leer pagina***********/

int buscarPaginaEnRam(tipoInstruccion instruccion, char* contenidoDePagina){
	return 1;
}

int buscarPaginaEnSwap(tipoInstruccion instruccion,char* contenidoDePagina, int socketSwap){
	return 1;
}

void enviarPaginaACPU(tipoInstruccion instruccion, char* contenidoDePagina){

}

void pedirPaginaDesdeSwapARam(tipoInstruccion isntruccion, char* contenidoDePagina, int socketSwap){

}

void leerPagina(tipoInstruccion instruccion, int socketSwap){

	char* contenidoDePagina;

	if (buscarPaginaEnRam(instruccion, contenidoDePagina)) {
		enviarPaginaACPU(instruccion, contenidoDePagina);

	} else if (buscarPaginaEnSwap(instruccion, contenidoDePagina, socketSwap)) {
			pedirPaginaDesdeSwapARam(instruccion, contenidoDePagina, socketSwap);
			enviarPaginaACPU(instruccion, contenidoDePagina);
			//gorenlpm, hace los scripts
		} else {
			//informar el error de alguna forma
		}
}

/************ESCRIBIR PAGINAS***************/
