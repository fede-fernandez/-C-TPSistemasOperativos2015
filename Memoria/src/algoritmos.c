/*
 * algoritmos.c
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#include "algoritmos.h"

void setearParaAlgoritmos(){

	datosMemoria->tipoDeAlgoritmoRAM = LRU;//Hardcodeado hasta que no se

	if(estaHabilitadaLaTLB())
		datosMemoria->colaFIFOTLB = list_create();//agregue en el archivo de config

}

tipoAccesoAPaginaTLB* cualReemplazarTLB(){

return cualReemplazarTLBFIFO();
}

int cualReemplazarRAM(tipoTablaPaginas* tablaDePaginas){

	int cualReemplazar;

	switch (datosMemoria->tipoDeAlgoritmoRAM) {

	/*case FIFO:
		cualReemplazar = cualReemplazarRAMFIFO(tablaDePaginas->listaParaAlgoritmo);
			break;*/

	case LRU:
		cualReemplazar = cualReemplazarRAMLRU(tablaDePaginas->listaParaAlgoritmo);
			break;

	case CLOCK_MODIFICADO:
		cualReemplazar = cualReemplazarRAMCLOCKM(tablaDePaginas);
			break;
	}

	return cualReemplazar;
}

int cualReemplazarRAMFIFO(t_list* listaAccesos){

	int* punteroANroPagina = list_get(listaAccesos,0);

	int paginaAReemplazar = *punteroANroPagina;

	list_remove(listaAccesos,0);

	return paginaAReemplazar;
}

tipoAccesoAPaginaTLB* cualReemplazarTLBFIFO(){

	int var,dondeEstaElAccesoAReemplazar;

	tipoAccesoAPaginaTLB* accesoAux;

	for (var = 0; var < list_size(datosMemoria->colaFIFOTLB); ++var) {

		accesoAux = list_get(datosMemoria->colaFIFOTLB,var);

		dondeEstaElAccesoAReemplazar = dondeEstaAccesoTLB(accesoAux->nroPagina,accesoAux->pid);

		if(dondeEstaElAccesoAReemplazar>=0)
			break;
	}

	list_remove(datosMemoria->colaFIFOTLB,dondeEstaElAccesoAReemplazar);

	return accesoAux;
}

int cualReemplazarRAMLRU(t_list* listaAccesos){

	int var,paginaAReemplazar = 0;

	int* accesoMasViejo = list_get(listaAccesos,0);

	int* aux ;

	for (var = 1; var < list_size(listaAccesos); ++var) {

		aux = list_get(listaAccesos,var);

		if(((*accesoMasViejo>*aux)||(*accesoMasViejo<0))&&(*aux>=0)){

			*accesoMasViejo = *aux;

			paginaAReemplazar = var;

		}
	}

	int* nuevoAcceso = malloc(sizeof(int));

	*nuevoAcceso = -1;

	list_replace_and_destroy_element(listaAccesos,paginaAReemplazar,nuevoAcceso,free);

	return paginaAReemplazar;
}

int cualReemplazarRAMCLOCKM(tipoTablaPaginas* tablaDePagina){

	int* nroPaginaAReemplazar = malloc(sizeof(int));

	bool noFunciono = false;

	while(noFunciono){

	noFunciono = !ejecutarPaso1CLOCKM(tablaDePagina,nroPaginaAReemplazar);

	if(noFunciono)
	noFunciono = !ejecutarPaso2CLOCKM(tablaDePagina,nroPaginaAReemplazar);
}

	return *nroPaginaAReemplazar;
}

bool ejecutarPaso1CLOCKM(tipoTablaPaginas* tablaDePagina,int* nroPaginaAReemplazar){

	int var,puntero = tablaDePagina->punteroParaAlgoritmo;

	bool encontrado = false;

	tipoPagina* pagina;

	int* acceso;

	for (var = 0; var < list_size(tablaDePagina->listaParaAlgoritmo); ++var) {

		if(puntero==list_size(tablaDePagina->listaParaAlgoritmo))
			puntero = 0;

		acceso = list_get(tablaDePagina->listaParaAlgoritmo,puntero);

		pagina = list_get(tablaDePagina->frames,*acceso);

		puntero++;

		if(!(pagina->usado||pagina->modificado)){

			encontrado = true;

			*nroPaginaAReemplazar = *acceso;

			break;
		}

	}

return encontrado;

}

bool ejecutarPaso2CLOCKM(tipoTablaPaginas* tablaDePagina,int* nroPaginaAReemplazar){

	int var,puntero = tablaDePagina->punteroParaAlgoritmo;

	bool encontrado = false;

	tipoPagina* pagina;

	int* acceso;

	for (var = 0; var < list_size(tablaDePagina->listaParaAlgoritmo); ++var) {

		if(puntero==list_size(tablaDePagina->listaParaAlgoritmo))
			puntero = 0;

		acceso = list_get(tablaDePagina->listaParaAlgoritmo,puntero);

		pagina = list_get(tablaDePagina->frames,*acceso);

		puntero++;

		if(!pagina->usado&&pagina->modificado){

			encontrado = true;

			*nroPaginaAReemplazar = *acceso;

			break;
		}

		modificarUso(*acceso,tablaDePagina->pid,false);
	}

return encontrado;

}

/*
int cualReemplazarTLBLRU(){

	return 0;
}

int cualReemplazarTLBCLOCKM(){

	return 0;
}*/

void agregarAccesoPorAlgoritmo(int nroPagina,int pid){

	tipoTablaPaginas* tablaDePaginas = traerTablaDePaginas(pid);

	switch (datosMemoria->tipoDeAlgoritmoRAM) {

	/*case FIFO:
				agregarAccesoPorFIFO(tablaDePaginas,nroPagina);
		break;*/

	case LRU:

		agregarAccesoPorLRU(tablaDePaginas,nroPagina);

			break;

	case CLOCK_MODIFICADO:
		agregarAccesoPorCLOCKM(tablaDePaginas,nroPagina);
			break;
	}

	if(estaHabilitadaLaTLB())
		agregarAccesoPorFIFO(nroPagina,pid);
}

void inicializarPorAlgoritmo(tipoTablaPaginas* tablaDePagina){

	switch (datosMemoria->tipoDeAlgoritmoRAM) {

		case LRU:
			inicializarPorLRU(tablaDePagina->listaParaAlgoritmo,tablaDePagina->paginasPedidas);
				break;

		}
}

void inicializarPorLRU(t_list* listaParaAlgoritmo,int paginasPedidas){
	int var;
	for (var = 0; var < paginasPedidas; ++var) {

		int* nuevoAcceso = malloc(sizeof(int));

		*nuevoAcceso = -1;

		list_add(listaParaAlgoritmo,nuevoAcceso);

	}
}

int ejecutarAlgoritmo(int* nroPagina,int pid,bool* estaModificada){

	tipoTablaPaginas* tablaDePaginas = traerTablaDePaginas(pid);

	int dondeEstaEnRam;

	int nroPaginaAReemplazar = cualReemplazarRAM(tablaDePaginas);

	*nroPagina = nroPaginaAReemplazar;

	tipoPagina* pagina = list_get(tablaDePaginas->frames,nroPaginaAReemplazar);

	*estaModificada = pagina->modificado;

	dondeEstaEnRam = buscarPagina(nroPaginaAReemplazar,pid);

	modificarDatosDePagina(nroPaginaAReemplazar,pid,-1,EN_SWAP,false,false);

	if(estaHabilitadaLaTLB()){
	quitarDeTLB(nroPaginaAReemplazar,pid);
	quitarAccesoTLB(nroPaginaAReemplazar,pid);
	}

	return dondeEstaEnRam;

}

void agregarAccesoPorFIFO(int nroPagina,int pid){

	if(dondeEstaAccesoTLB(nroPagina,pid)<0){

	tipoAccesoAPaginaTLB* nuevoAcceso = malloc(sizeof(tipoAccesoAPaginaTLB));

	nuevoAcceso->nroPagina = nroPagina;

	nuevoAcceso->pid = pid;

	list_add(datosMemoria->colaFIFOTLB,nuevoAcceso);

	}
}

void agregarAccesoPorLRU(tipoTablaPaginas* tablaDePaginas,int nroPagina){

	int* nuevoAcceso = malloc(sizeof(int));

	tablaDePaginas->punteroParaAlgoritmo++;

	nuevoAcceso= malloc(sizeof(int));

	*nuevoAcceso = tablaDePaginas->punteroParaAlgoritmo;

	list_replace_and_destroy_element(tablaDePaginas->listaParaAlgoritmo,nroPagina,nuevoAcceso,free);
}

void agregarAccesoPorCLOCKM(tipoTablaPaginas* tablaDePaginas,int nroPagina){

	bool paginaYaAccedida = false;

	int var;
	for (var = 0; var < list_size(tablaDePaginas->listaParaAlgoritmo); ++var) {

		int* acceso = list_get(tablaDePaginas->listaParaAlgoritmo,var);

		if(*acceso==nroPagina){

			paginaYaAccedida = true;

			break;
		}
	}

	if(paginaYaAccedida){

		int* nuevoAcceso = malloc(sizeof(int));

		*nuevoAcceso = nroPagina;

		list_add(tablaDePaginas->listaParaAlgoritmo,nuevoAcceso);
	}

}

void quitarAccesoTLB(int nroPagina,int pid){

	int dondeEstaAcceso = dondeEstaAccesoTLB(nroPagina,pid);

	if(dondeEstaAcceso>=0)
		list_remove_and_destroy_element(datosMemoria->colaFIFOTLB,dondeEstaAcceso,free);
}

int dondeEstaAccesoTLB(int nroPagina,int pid){

	tipoAccesoAPaginaTLB* acceso;

	int var,posicionDeAcceso = -1;

	for (var = 0; var < list_size(datosMemoria->colaFIFOTLB); ++var) {

		acceso = list_get(datosMemoria->colaFIFOTLB,var);

		if(acceso->nroPagina==nroPagina&&acceso->pid==pid){

			posicionDeAcceso = var;

			break;
		}
	}

	return posicionDeAcceso;
}








