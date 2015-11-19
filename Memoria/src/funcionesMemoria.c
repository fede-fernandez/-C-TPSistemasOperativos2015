#include "funcionesMemoria.h"
#include <commonsDeAsedio/estructuras.h>
#include <commons/string.h>



void setearEstructuraMemoria(tipoEstructuraMemoria* datos) {

	datosMemoria = datos;

	datosMemoria->listaTablaPaginas = list_create();

	datosMemoria->listaRAM = list_create();

	if(estaHabilitadaLaTLB())
	datosMemoria->listaTLB = list_create();

	setearParaAlgoritmos();

	iniciarListaHuecosRAM();

}


/************************FUNCIONES********************************/

void iniciarListaHuecosRAM(){

	datosMemoria->listaHuecosRAM = list_create();

	int var;
	for (var = 0; var < datosMemoria->configuracion->cantidadDeMarcos; ++var) {

		bool* hueco = malloc(sizeof(bool));

		*hueco = true;

		list_add(datosMemoria->listaHuecosRAM,hueco);

		char* contenidoHueco = string_duplicate("");

		list_add(datosMemoria->listaRAM,contenidoHueco);
	}

}

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

	int posicionDeTabla = buscarTabla(instruccion.pid);//Esto lo hago xq tengo que consultar si existe el proceso
	//y ya que estamos de paso si existe en vez de buscarlo de nuevo hago un list_get de la posicion encontrada

	if(posicionDeTabla<0)
		return crearTipoRespuesta(MANQUEADO,"Proceso no existente");

	if(instruccionASwapRealizada(&instruccion,&respuesta)){//Aca swap me devolvio todo ok aunque el proceso no existia!!

		tipoTablaPaginas* tablaDePaginas = list_get(datosMemoria->listaTablaPaginas,posicionDeTabla);//traerTablaDePaginas(instruccion.pid);

		tipoPagina* pagina;

		int var;
		for (var = 0; var < list_size(tablaDePaginas->frames); ++var) {

			quitarDeTLB(instruccion.nroPagina,instruccion.pid);

			pagina = list_get(tablaDePaginas->frames,var);

			quitarPaginaDeRam(pagina->posicionEnRAM);
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

		modificarUso(instruccion.nroPagina,instruccion.pid,true);

		agregarAccesoPorAlgoritmo(instruccion.nroPagina,instruccion.pid);

		if(estaHabilitadaLaTLB())
		agregarPaginaATLB(instruccion.nroPagina,instruccion.pid,posicionEnRam);
	}

	else
		respuesta = crearTipoRespuesta(MANQUEADO,"Pagina no existente");

	return respuesta;
}

int buscarPagina(int nroPagina,int pid){

	int posicionDePag = -1;

	if(estaHabilitadaLaTLB())
		posicionDePag = buscarPaginaEnTLB(nroPagina,pid);

	if(posicionDePag<0){

		posicionDePag = buscarPaginaEnTabla(nroPagina,pid);
		}

	if(posicionDePag==EN_SWAP){

		printf("Pagina buscada en Swap..\n");

	tipoRespuesta* respuesta;

	tipoInstruccion* instruccion = crearTipoInstruccion(pid,LEER,nroPagina,"");

	posicionDePag = traerPaginaDesdeSwap(*instruccion,&respuesta);

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

	tipoTablaPaginas* tabla = list_get(datosMemoria->listaTablaPaginas,posicionDeTabla);

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

int traerPaginaDesdeSwap(tipoInstruccion instruccion, tipoRespuesta** respuesta) {

	//instruccion.instruccion = LEER;

	int posicionEnRam = -1;

	if(instruccionASwapRealizada(&instruccion,respuesta)){
		printf("Pagina traida de swap!!");

		char* nuevaPagina = string_duplicate((*respuesta)->informacion);

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

		posicionDePag = agregarPagina(instruccion.nroPagina,instruccion.pid,string_duplicate(instruccion.texto));

		aumentarPaginasAsignadas(instruccion.pid);
	}

	else {
		modificarPagina(instruccion.nroPagina,instruccion.pid,posicionDePag,instruccion.texto);

		modificarUso(instruccion.nroPagina,instruccion.pid,true);
	}

	modificarModificado(instruccion.nroPagina,instruccion.pid,true);

	agregarAccesoPorAlgoritmo(instruccion.nroPagina,instruccion.pid);

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

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	tipoPagina* paginaAModificar = list_get(tablaDePagina->frames,nroPagina);

	paginaAModificar->usado = uso;

}

void agregarPaginaATLB(int nroPagina,int pid,int posicionEnRam){


	if(buscarPaginaEnTLB(nroPagina,pid)>=0)
		return;

	if(TLBLlena()){

		/*tipoAccesoAPaginaTLB*  cualReemplazar = cualReemplazarTLB();

		int posicionAReemplazar = dondeEstaPaginaEnTLB(cualReemplazar->nroPagina,cualReemplazar->pid);*/

		list_remove_and_destroy_element(datosMemoria->listaTLB,0/*posicionAReemplazar*/,free);//Como siempre agrego al final,la primera va a ser la q hay q sacar
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

	printf("Entrando a agregar pagina..\n");

	int posicionEnRam;

	if(RAMLlena()||excedeMaximoDeMarcos(pid)){

		int nroPaginaAReemplazar;// = malloc(sizeof(int));

		bool estaModificada;

		posicionEnRam = ejecutarAlgoritmo(&nroPaginaAReemplazar,pid,&estaModificada);

		if(estaModificada)
			llevarPaginaASwap(nroPaginaAReemplazar,pid,posicionEnRam);

	}

	else {

		printf("Detecto que hay espacio en ram..\n");

		posicionEnRam = buscarHuecoRAM();//list_size(datosMemoria->listaRAM);

		setearHuecoEnListaHuecosRAM(posicionEnRam,false);

		printf("La posicion vacia de ram es :%d\n",posicionEnRam);
	}

	list_replace_and_destroy_element(datosMemoria->listaRAM,posicionEnRam,contenido,free);//Esto es nuevo porque para manejar los huecos la ram ya tiene q estar inicializada con ""

	printf("No rompio en list_replace..\n");

	modificarDatosDePagina(nroPagina,pid,posicionEnRam,EN_RAM,true,false);

	return posicionEnRam;
}

void llevarPaginaASwap(int nroPaginaAReemplazar,int pid,int posicionEnRam){

	tipoRespuesta* respuesta;

			tipoInstruccion* instruccionASwap = crearTipoInstruccion(pid,ESCRIBIR,nroPaginaAReemplazar,string_duplicate(traerPaginaDesdeRam(posicionEnRam)));

			if(instruccionASwapRealizada(instruccionASwap,&respuesta))
				modificarDatosDePagina(nroPaginaAReemplazar,pid,-1,EN_SWAP,false,false);

			free(instruccionASwap);//esto puede romper porque lo agregue a lo ultimo..

			free(respuesta);

			printf("La posicion en la que se escribira la pagina es:%d\n",posicionEnRam);

}

void modificarDatosDePagina(int nroPagina,int pid,int posicionEnRam,int presente,bool uso,bool modificado){

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	tipoPagina* paginaNueva = crearPagina(nroPagina,posicionEnRam,presente,uso,modificado);

	list_replace_and_destroy_element(tablaDePagina->frames,nroPagina,paginaNueva,free);

}

void quitarDeTLB(int nroPagina,int pid){

	printf("Entre a quitar de tlb..\n");

int posicionEnTLB = dondeEstaPaginaEnTLB(nroPagina,pid);

printf("La posicion en tlb es :%d\n",posicionEnTLB);

	if(posicionEnTLB>=0)
		list_remove_and_destroy_element(datosMemoria->listaTLB,posicionEnTLB,free);

}

void quitarTablaDePaginas(int pid){

	tipoTablaPaginas* tablaDePaginas = traerTablaDePaginas(pid);

	list_destroy_and_destroy_elements(tablaDePaginas->frames,free);

	list_destroy_and_destroy_elements(tablaDePaginas->listaParaAlgoritmo,free);

	list_remove_and_destroy_element(datosMemoria->listaTablaPaginas,buscarTabla(pid),free);

}

void quitarPaginaDeRam(int posicion){

	if(posicion>=0){
		list_replace_and_destroy_element(datosMemoria->listaRAM,posicion,string_duplicate(""),free);
		setearHuecoEnListaHuecosRAM(posicion,true);
		}
	}

void setearHuecoEnListaHuecosRAM(int posicion,bool estado){

bool* hueco = list_get(datosMemoria->listaHuecosRAM,posicion);

*hueco = estado;
}

int dondeEstaPaginaEnTLB(int nroPagina,int pid){

	tipoTLB* instanciaTLB;

	int var,dondeEsta=-1;
	for (var = 0; var < list_size(datosMemoria->listaTLB); ++var) {

		instanciaTLB = list_get(datosMemoria->listaTLB,var);

		if(instanciaTLB->numeroDePagina==nroPagina&&instanciaTLB->pid==pid){

			dondeEsta = var;

			break;
		}
	}

	return dondeEsta;
}

int buscarHuecoRAM(){

	int var;

	bool* hueco;

	for (var = 0; var < list_size(datosMemoria->listaHuecosRAM); ++var) {

		hueco = list_get(datosMemoria->listaHuecosRAM,var);

		if(*hueco)
			return var;
	}

	return -1;
}
