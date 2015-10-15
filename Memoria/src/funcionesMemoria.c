#include "funcionesMemoria.h"
#include <commonsDeAsedio/estructuras.h>


void destruirConfigMemoria(tipoConfigMemoria* estructuraDeConfiguracion){
	free(estructuraDeConfiguracion->ipSWAP);
	free(estructuraDeConfiguracion->TLBHabilitada);
	free(estructuraDeConfiguracion);
}

tipoConfigMemoria* crearConfigMemoria(){
	tipoConfigMemoria* cfg = malloc(sizeof(tipoConfigMemoria));
	cfg->ipSWAP = string_new();
	cfg->TLBHabilitada = string_new();

	return cfg;
}

/*********************ESTRUCTURAS*************************/

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

void setearEstructuraMemoria(tipoEstructuraMemoria* datos){

	datosMemoria = datos;

}

/************************FUNCIONES********************************/

void tratarPeticion(int cpuAtendida){

	tipoInstruccion* instruccion = recibirInstruccion(cpuAtendida);

	switch (instruccion->instruccion) {
		case INICIAR:
			reservarMemoriaParaProceso(*instruccion,cpuAtendida);
			break;

		case LEER:

			break;

		case ESCRIBIR:

			break;

		case ENTRADA_SALIDA:

			break;

		case FINALIZAR:
			break;
	}
}

void tratarPeticiones(){

	int var;
	for (var = 1; var <datosMemoria->maximoSocket; ++var) {
		if(FD_ISSET(var,datosMemoria->cpusATratar))
		tratarPeticion(var);
		}
}

/**************INSTRUCCIONES*******************/

//////////////////
//INICIAR
/////////////////

int puedoReservarEnSWAP(tipoInstruccion instruccion, tipoRespuesta* respuesta){
	enviarInstruccion(datosMemoria->socketSWAP,instruccion);
		respuesta = recibirRespuesta(datosMemoria->socketSWAP);

		if (respuesta->respuesta == PERFECTO)return 1;
		else return 0;

}

int puedoReservarEnRAM(tipoInstruccion instruccion){

	int paginasTotales = instruccion.nroPagina + list_size(datosMemoria->listaRAM);

	if(paginasTotales <= datosMemoria->configuracion->cantidadDeMarcos && instruccion.nroPagina <= datosMemoria->configuracion->maximoDeMarcosPorProceso) return 1;
	else return 0;


	//algoritmo fifo LRU
}

void reservarMemoriaEnRam(tipoInstruccion instruccion){

	//RAM
	//int i;
	if(puedoReservarEnRAM(instruccion)==1){

		tipoRAM* instanciaDeRAM = malloc(datosMemoria->configuracion->tamanioDeMarco);

	instanciaDeRAM->instruccion = instruccion.pid;

	instanciaDeRAM->numeroDePagina = obtenerUltimaPagina(instruccion.pid)+1;

	list_add(datosMemoria->listaRAM,instanciaDeRAM);

	//TLB
	if(estaHabilitadaLaTLB(datosMemoria->configuracion)){

		tipoTLB* instanciaTLB = malloc(sizeof(tipoTLB));
			instanciaTLB->instruccion = instruccion.pid;
			instanciaTLB->numeroDePagina = instruccion.nroPagina;
			instanciaTLB->posicionEnRAM = obtenerPosicionEnRAM(instanciaDeRAM);

		reservarMemoriaEnTLB(instanciaTLB);
	}
}



	/*for (i = 1; i <= instruccion.nroPagina; ++i) {

		instanciaDeRAM = malloc(sizeof(tipoRAM));
			instanciaDeRAM->numeroDePagina = i;
			instanciaDeRAM->instruccion = instruccion.pid;

		list_add(datosMemoria->listaRAM, instanciaDeRAM);
	}/*///Creo que esta mal


}

int obtenerUltimaPagina(int pid){

	int var,contador;

	tipoRAM* instanciaRamActual;

	for (var = list_size(datosMemoria->listaRAM)-1;var>=0; ++var) {

		instanciaRamActual = list_get(datosMemoria->listaRAM,var);

		if(instanciaRamActual->instruccion==pid)
			break;
	}

	return instanciaRamActual->numeroDePagina;
}

int estaHabilitadaLaTLB(){
	return string_equals_ignore_case(datosMemoria->configuracion->TLBHabilitada, "SI");
}

int obtenerPosicionEnRAM(tipoRAM* instanciaRAM){
	tipoRAM* aux;
	int i;
	for(i=0; i < list_size(datosMemoria->listaRAM); ++i) {
		aux = list_get(datosMemoria->listaRAM, i);

		if(aux->instruccion == instanciaRAM->instruccion  && aux->numeroDePagina == instanciaRAM->numeroDePagina){
			break; //ver si funciona porque no lo pude hacer de otra forma, C no me deja
		}
	}
	return i;
}

void reservarMemoriaEnTLB(tipoTLB* instanciaTLB){

	if(list_size(datosMemoria->listaTLB)< datosMemoria->configuracion->entradasDeTLB){
		list_add(datosMemoria->listaTLB, instanciaTLB);
	}
	else{
		//aca irian las funciones para actualizar la TLB sacando alguno viejo y poniendo el nuevo
		//despues se lo tiene que ver
	}
}

void reservarMemoriaParaProceso(tipoInstruccion instruccion, int cpuATratar){

	tipoRespuesta* respuesta;

	if (puedoReservarEnSWAP(instruccion,respuesta)) {

		if (puedoReservarEnRAM(instruccion))
			reservarMemoriaEnRam(instruccion);
		else{
			//ver algoritmos para pasar procesos de ram a swap y liberar espacio para el nuevo proceso
		}
	}

	enviarRespuesta(cpuATratar, *respuesta);
}

//////////////////
//LEER PAGINA
/////////////////

int buscarPaginaEnRam(tipoInstruccion instruccion, char* contenidoDePagina){

	int var;

	tipoRAM* instanciaRamActual;

	for (var = 0; var < list_size(datosMemoria->listaRAM); ++var) {//Necesito que me digan si esto buscaba la posicion de la pagina o q

		instanciaRamActual = list_get(datosMemoria->listaRAM,var);

		if(instanciaRamActual->contenido==contenidoDePagina&&instanciaRamActual->instruccion==instruccion.pid&&instanciaRamActual->numeroDePagina==instruccion.nroPagina)
			break;
	}
	return var;
}

int buscarPaginaEnSwap(tipoInstruccion instruccion,char* contenidoDePagina, int socketSwap){
	//Para que mierda es esta funcion??
	return 1;
}

void enviarPaginaACPU(char* contenidoDePagina,int socketCpu){

tipoRespuesta respuestaCpu;

respuestaCpu.informacion = contenidoDePagina;

respuestaCpu.respuesta = PERFECTO;

enviarRespuesta(socketCpu,respuestaCpu);

}

void pedirPaginaDesdeSwapARam(tipoInstruccion instruccion, char* contenidoDePagina, int socketSwap){

}

void leerPagina(tipoInstruccion instruccion,int socketCpu){

	char* contenidoDePagina;

	if (buscarPaginaEnRam(instruccion, contenidoDePagina)) {
		enviarPaginaACPU(contenidoDePagina,socketCpu);

	} else if (buscarPaginaEnSwap(instruccion, contenidoDePagina, datosMemoria->socketSWAP)) {
			pedirPaginaDesdeSwapARam(instruccion, contenidoDePagina, datosMemoria->socketSWAP);
			enviarPaginaACPU(contenidoDePagina,socketCpu);//La verdad no se que queria hacer brian aca pero hay codigo repetido!!
			//gorenlpm, hace los scripts
		} else {
			//informar el error de alguna forma
		}
}

//////////////////
//ESCRIBIR PAGINA
/////////////////

/*

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
 */


void* tuvieja;

int tamanio = sizeof(*tuvieja);





