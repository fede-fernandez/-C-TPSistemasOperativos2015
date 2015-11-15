#include "funcionesMemoria.h"
#include <commonsDeAsedio/estructuras.h>
#include <commons/string.h>



void setearEstructuraMemoria(tipoEstructuraMemoria* datos) {

	datosMemoria = datos;

	datosMemoria->listaTablaPaginas = list_create();

	datosMemoria->listaRAM = list_create();

	datosMemoria->listaAccesosAPaginasRAM = list_create();

	if(estaHabilitadaLaTLB())
	datosMemoria->listaTLB = list_create();

	datosMemoria->listaAccesosAPaginasRAM = list_create();

	setearParaAlgoritmos();

	//setearParaValidaciones(datosMemoria);
}


/************************FUNCIONES********************************/

void tratarPeticion(int cpuAtendida) {

	tipoInstruccion* instruccion = recibirInstruccion(cpuAtendida);

	tipoRespuesta* respuesta;

	switch (instruccion->instruccion) {
	case INICIAR:
		respuesta = iniciar(*instruccion);
		break;

	case LEER:
		respuesta = leerPagina(*instruccion);
		break;

	case ESCRIBIR:
			respuesta = escribirPagina(*instruccion);

		break;

	case FINALIZAR:
		respuesta = quitarProceso(*instruccion);
		break;
	}

	enviarRespuesta(cpuAtendida,respuesta);

	free(respuesta);
}

tipoRespuesta* quitarProceso(tipoInstruccion instruccion){

	tipoRespuesta* respuesta ;

	if(instruccionASwapRealizada(&instruccion,&respuesta)){

		tipoTablaPaginas* tablaDePaginas = traerTablaDePaginas(instruccion.pid);

		int var;
		for (var = 0; var < list_size(tablaDePaginas->frames); ++var) {

			quitarDeTLB(instruccion.nroPagina,instruccion.pid);

			quitarPaginaDeRam(instruccion.nroPagina,instruccion.pid);
		}

		quitarTablaDePaginas(instruccion.pid);
	}

	return respuesta;
}


/**************INSTRUCCIONES*******************/

//////////////////
//INICIAR
/////////////////
tipoRespuesta* iniciar(tipoInstruccion instruccion) {

	tipoRespuesta* respuesta;

	if(procesoExiste(instruccion.pid))
		return crearTipoRespuesta(MANQUEADO,"Tabla de paginas de proceso ya existente");

	if (puedoReservarEnSWAP(instruccion, &respuesta)) {

		tipoTablaPaginas* tablaDePaginasNueva = malloc(sizeof(tipoTablaPaginas));

		tablaDePaginasNueva->frames = list_create();

		tablaDePaginasNueva->paginasAsignadas = 0;

		tablaDePaginasNueva->listaParaAlgoritmo = list_create();

		tablaDePaginasNueva->paginasPedidas = instruccion.nroPagina;

		tablaDePaginasNueva->pid = instruccion.pid;

		tablaDePaginasNueva->punteroParaAlgoritmo = 0;

		inicializarPaginas(tablaDePaginasNueva);

		inicializarPorAlgoritmo(tablaDePaginasNueva);

		list_add(datosMemoria->listaTablaPaginas, tablaDePaginasNueva);

			}

	return respuesta;
}



void inicializarPaginas(tipoTablaPaginas* tablaDePaginasNueva){

	int var;
	for (var = 0; var < tablaDePaginasNueva->paginasPedidas; ++var) {

		agregarATablaDePaginas(tablaDePaginasNueva,var,-1,NO_EXISTE,false,false);
	}
}

tipoPagina* crearPagina(int nroPagina,int posEnRAM,char presente,bool usado,bool modificado){

	tipoPagina* nuevaPagina = malloc(sizeof(tipoPagina));

	nuevaPagina->modificado = modificado;

	nuevaPagina->posicionEnRAM = posEnRAM;

	nuevaPagina->presente = presente;

	nuevaPagina->usado = usado;

	return nuevaPagina;

}

void agregarATablaDePaginas(tipoTablaPaginas* tablaDePaginas,int nroPagina,int posEnRAM,char presente,bool usado,bool modificado){

	tipoPagina* nuevaPagina = crearPagina(nroPagina,posEnRAM,presente,usado, modificado);

	list_add_in_index(tablaDePaginas->frames,nroPagina,nuevaPagina);
}

bool instruccionASwapRealizada(tipoInstruccion* instruccion,tipoRespuesta** respuesta) {


	enviarInstruccion(datosMemoria->socketSWAP, instruccion);

	*respuesta = recibirRespuesta(datosMemoria->socketSWAP);

	return ((*respuesta)->respuesta == PERFECTO);
}

bool puedoReservarEnSWAP(tipoInstruccion instruccion, tipoRespuesta** respuesta) {


	return instruccionASwapRealizada(&instruccion, respuesta);
}

tipoRespuesta* leerPagina(tipoInstruccion instruccion){


	tipoRespuesta* respuesta;

	if(!procesoExiste(instruccion.pid))
		return crearTipoRespuesta(MANQUEADO,"Tabla de paginas de proceso no existente");

	if(numeroDePaginaIncorrecto(instruccion.nroPagina,instruccion.pid))
		return crearTipoRespuesta(MANQUEADO,"Numero de pagina excede el maximo numero");


	int posicionEnRam = buscarPagina(instruccion.nroPagina,instruccion.pid);

	if(posicionEnRam>=0){

		char* pagina = traerPaginaDesdeRam(posicionEnRam);

		respuesta = crearTipoRespuesta(PERFECTO,pagina);

		printf("La pagina ya se encontro en ram..\n");

		agregarAccesoPorAlgoritmo(instruccion.nroPagina,instruccion.pid);

		modificarUso(instruccion.nroPagina,instruccion.pid,true);

		if(estaHabilitadaLaTLB())
		agregarPaginaATLB(instruccion.nroPagina,instruccion.pid,posicionEnRam);
	}

	else
		respuesta = crearTipoRespuesta(MANQUEADO,"Pagina no existente");

	return respuesta;
}

int buscarPagina(int nroPagina,int pid){


	int posicionDePag;

	if(estaHabilitadaLaTLB())
		posicionDePag = buscarPaginaEnTLB(nroPagina,pid);

	if(posicionDePag<0){

		posicionDePag = buscarPaginaEnTabla(nroPagina,pid);
		}

	if(posicionDePag==EN_SWAP){

		printf("Pagina buscada en Swap..\n");

	tipoRespuesta* respuesta;

	tipoInstruccion* instruccion = crearTipoInstruccion(pid,LEER,nroPagina,"");

	posicionDePag = traerPaginaDesdeSwap(*instruccion,respuesta);

	printf("Saliendo de buscarPagina..\n");

	free(respuesta);

	}



	return posicionDePag;
}

int buscarPaginaEnTLB(int nroPagina,int pid){


	int var, posicionDePagina = -1;

	tipoTLB* estructuraTLBActual;

	for (var = 0; var < list_size(datosMemoria->listaTLB); ++var) {

		estructuraTLBActual = list_get(datosMemoria->listaTLB, var);

		if (estructuraTLBActual->numeroDePagina == nroPagina&& estructuraTLBActual->pid == pid) {

			posicionDePagina = estructuraTLBActual->posicionEnRAM;

			break;
		}
	}

	return posicionDePagina;
}

int buscarPaginaEnTabla(int nroPagina,int pid){//Me hace ruido que no haya que hacer sleep

		tipoTablaPaginas* tablaActual = traerTablaDePaginas(pid);

		tipoPagina* paginaActual = list_get(tablaActual->frames,nroPagina);

		if(paginaActual->presente!=EN_RAM)
			return paginaActual->presente;

		else return paginaActual->posicionEnRAM;
}

tipoTablaPaginas* traerTablaDePaginas(pid){


	int posicionDeTabla = buscarTabla(pid);

	printf("Aca no rompio..\n");

	tipoTablaPaginas* tabla = list_get(datosMemoria->listaTablaPaginas,posicionDeTabla);

	printf("Ya deberia de finalizar..\n");

	return tabla;
}



//////////////////
//LEER PAGINA
/////////////////

char* traerPaginaDesdeRam(int direccion){

	sleep(datosMemoria->configuracion->retardoDeMemoria);

	char* pagina = list_get(datosMemoria->listaRAM,direccion);

	return pagina;
}

int buscarTabla(int pid){

int var,posicionDeTabla = -1;

tipoTablaPaginas* tablaActual;

for (var = 0; var < list_size(datosMemoria->listaTablaPaginas); ++var) {

	tablaActual = list_get(datosMemoria->listaTablaPaginas,var);

	if(tablaActual->pid==pid){

		posicionDeTabla = var;

		break;
	}
}

return posicionDeTabla;

}

int traerPaginaDesdeSwap(tipoInstruccion instruccion, tipoRespuesta* respuesta) {

	//instruccion.instruccion = LEER;

	int posicionEnRam = -1;

	if(instruccionASwapRealizada(&instruccion,&respuesta)){
		printf("Pagina traida de swap!!");

		char* nuevaPagina = string_duplicate(respuesta->informacion);

		posicionEnRam = agregarPagina(instruccion.nroPagina,instruccion.pid,nuevaPagina);
	}

	return posicionEnRam;
}


//////////////////
//ESCRIBIR PAGINA
/////////////////



tipoRespuesta* escribirPagina(tipoInstruccion instruccion){

	if(!procesoExiste(instruccion.pid))
		return crearTipoRespuesta(MANQUEADO,"Tabla de paginas de proceso no existente");

	if(tamanioDePaginaMayorAlSoportado(instruccion.texto))
		return crearTipoRespuesta(MANQUEADO,"Tamaño de pagina mayor al de marco");

	if(numeroDePaginaIncorrecto(instruccion.nroPagina,instruccion.pid))
		return crearTipoRespuesta(MANQUEADO,"Numero de pagina excede el maximo numero");

	int posicionDePag = buscarPagina(instruccion.nroPagina,instruccion.pid);

	sleep(datosMemoria->configuracion->retardoDeMemoria);//tengo q tardar tanto como si la creo como si la modifico

	if(posicionDePag==NO_EXISTE){

		printf("Pagina no existe..\n");

		posicionDePag = agregarPagina(instruccion.nroPagina,instruccion.pid,instruccion.texto);

		aumentarPaginasAsignadas(instruccion.pid);
	}

	else
		modificarPagina(instruccion.nroPagina,instruccion.pid,posicionDePag,instruccion.texto);

	agregarAccesoPorAlgoritmo(instruccion.nroPagina,instruccion.pid);

	modificarUso(instruccion.nroPagina,instruccion.pid,true);

	modificarModificado(instruccion.nroPagina,instruccion.pid,true);

	if(estaHabilitadaLaTLB())
	agregarPaginaATLB(instruccion.nroPagina,instruccion.pid,posicionDePag);

	return crearTipoRespuesta(PERFECTO,instruccion.texto);
}

void aumentarPaginasAsignadas(int pid){

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	tablaDePagina->paginasAsignadas++;
}

void modificarPagina(int nroPagina,int pid,int posicionDePag,char* texto){

	list_replace(datosMemoria->listaRAM,posicionDePag,string_duplicate(texto));
}

void modificarModificado(int nroPagina,int pid,bool modificado){

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	tipoPagina* paginaAModificar = list_get(tablaDePagina->frames,nroPagina);

	paginaAModificar->modificado = modificado;

}

void modificarUso(int nroPagina,int pid,bool uso){

	printf("Voy a modificar uso..\n");

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	tipoPagina* paginaAModificar = list_get(tablaDePagina->frames,nroPagina);

	printf("Uso siendo modificado..\n");

	paginaAModificar->usado = uso;

	printf("Uso modificado..\n");
}

void agregarPaginaATLB(int nroPagina,int pid,int posicionEnRam){


	if(buscarPaginaEnTLB(nroPagina,pid)>=0)
		return;

	if(TLBLlena()){

		int posicionAReemplazar = cualReemplazarTLB();

		list_remove(datosMemoria->listaTLB,posicionAReemplazar);
	}

	tipoTLB* instanciaTLB = malloc(sizeof(tipoTLB));

	instanciaTLB->numeroDePagina = nroPagina;

	instanciaTLB->pid = pid;

	instanciaTLB->posicionEnRAM = posicionEnRam;

	list_add(datosMemoria->listaTLB,instanciaTLB);
}




void volcarRamALog(){

//Aca decia que hay que usar fork, pero me parece quilombo para nada
}


int agregarPagina(int nroPagina,int pid,char* contenido){

	int posicionEnRam;

	if(RAMLlena()||excedeMaximoDeMarcos(pid)){

		int* nroPaginaAReemplazar = malloc(sizeof(int));

		bool estaModificada;

		posicionEnRam = ejecutarAlgoritmo(nroPaginaAReemplazar,pid,&estaModificada);

		if(estaModificada){

		tipoRespuesta* respuesta;

		tipoInstruccion* instruccionASwap = crearTipoInstruccion(pid,ESCRIBIR,*nroPaginaAReemplazar,string_duplicate(traerPaginaDesdeRam(posicionEnRam)));

		instruccionASwapRealizada(instruccionASwap,&respuesta);

		free(instruccionASwap);//esto puede romper porque lo agregue a lo ultimo..

		}

		list_replace_and_destroy_element(datosMemoria->listaRAM,posicionEnRam,contenido,free);
	}

	else {
		posicionEnRam = list_size(datosMemoria->listaRAM);

		list_add_in_index(datosMemoria->listaRAM,posicionEnRam,contenido);
	}

	modificarDatosDePagina(nroPagina,pid,posicionEnRam,EN_RAM,true,false);

	return posicionEnRam;
}


void modificarDatosDePagina(int nroPagina,int pid,int posicionEnRam,int presente,bool uso,bool modificado){

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	tipoPagina* paginaNueva = crearPagina(nroPagina,posicionEnRam,presente,uso,modificado);

	list_replace_and_destroy_element(tablaDePagina->frames,nroPagina,paginaNueva,free);

}

void quitarDeTLB(int nroPagina,int pid){

	tipoTLB* instanciaTLB;

	int var;
	for (var = 0; var < list_size(datosMemoria->listaTLB); ++var) {

		instanciaTLB = list_get(datosMemoria->listaTLB,var);

		if(instanciaTLB->pid==pid&&instanciaTLB->numeroDePagina==nroPagina){

			list_remove(datosMemoria->listaTLB,var);

			break;
		}

	}
}

void quitarTablaDePaginas(int pid){

	tipoTablaPaginas* tablaDePaginas = traerTablaDePaginas(pid);

	list_destroy_and_destroy_elements(tablaDePaginas->frames,free);

	list_destroy_and_destroy_elements(tablaDePaginas->listaParaAlgoritmo,free);

	list_remove_and_destroy_element(datosMemoria->listaTablaPaginas,buscarTabla(pid),free);

}

void quitarPaginaDeRam(int nroPagina,int pid){//Aca hay un problema con direcciones invalidas de otras paginas

	int dondeEstaEnRam = buscarPaginaEnTabla(nroPagina,pid);

	if(dondeEstaEnRam!=EN_SWAP&&dondeEstaEnRam!=NO_EXISTE)
		list_replace_and_destroy_element(datosMemoria->listaRAM,dondeEstaEnRam,"",free);//esto despues hay q verlo pero lo hago para dejar un hueco en ram
																						//y que no se modifiquen las posiciones de las paginas
	}


