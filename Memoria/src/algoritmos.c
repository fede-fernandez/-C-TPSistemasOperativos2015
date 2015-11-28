/*
 * algoritmos.c
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#include "algoritmos.h"

void setearParaAlgoritmos(){

	if(string_equals_ignore_case(datosMemoria->configuracion->algoritmoRAM,"FIFO"))
		datosMemoria->tipoDeAlgoritmoRAM = FIFO;

	if(string_equals_ignore_case(datosMemoria->configuracion->algoritmoRAM,"LRU"))
		datosMemoria->tipoDeAlgoritmoRAM = LRU;

	if(string_equals_ignore_case(datosMemoria->configuracion->algoritmoRAM,"CLOCK_MODIFICADO"))
		datosMemoria->tipoDeAlgoritmoRAM = CLOCK_MODIFICADO;

	/*if(estaHabilitadaLaTLB())
		datosMemoria->colaFIFOTLB = list_create();//agregue en el archivo de config*/

}

/*tipoAccesoAPaginaTLB* cualReemplazarTLB(){

return cualReemplazarTLBFIFO();
}*/

int cualReemplazarRAM(tipoTablaPaginas* tablaDePaginas){

	int cualReemplazar;

	switch (datosMemoria->tipoDeAlgoritmoRAM) {

	case FIFO:
		cualReemplazar = cualReemplazarRAMFIFO(tablaDePaginas->listaParaAlgoritmo);
			break;

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

	list_remove_and_destroy_element(listaAccesos,0,free);

	return paginaAReemplazar;
}

/*tipoAccesoAPaginaTLB* cualReemplazarTLBFIFO(){

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
}*/

int cualReemplazarRAMLRU(t_list* listaAccesos){

	int var,paginaAReemplazar = 0;

	int* accesoMasViejo = list_get(listaAccesos,0);

	int* aux;

	for (var = 1; var < list_size(listaAccesos); ++var) {

		aux = list_get(listaAccesos,var);

		if(((*accesoMasViejo>*aux)||(*accesoMasViejo<0))&&(*aux>=0)){

			accesoMasViejo = aux;

			paginaAReemplazar = var;

		}
	}

	*accesoMasViejo = -1;

	/*int* nuevoAcceso = malloc(sizeof(int));

	*nuevoAcceso = -1;

	list_replace_and_destroy_element(listaAccesos,paginaAReemplazar,nuevoAcceso,free);
*/
	return paginaAReemplazar;
}

int cualReemplazarRAMCLOCKM(tipoTablaPaginas* tablaDePagina){

	int nroPaginaAReemplazar;

	bool noFunciono = true;

	while(noFunciono){

	noFunciono = !ejecutarPaso1CLOCKM(tablaDePagina,&nroPaginaAReemplazar);

	if(noFunciono)
	noFunciono = !ejecutarPaso2CLOCKM(tablaDePagina,&nroPaginaAReemplazar);
}

	return nroPaginaAReemplazar;
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

		if(!(pagina->usado||pagina->modificado)){

			encontrado = true;

			*nroPaginaAReemplazar = *acceso;

			*acceso = -1;

			break;
		}

		puntero++;
	}

	if(encontrado)
	puntero++;

	if(puntero>=list_size(tablaDePagina->listaParaAlgoritmo))
		puntero = 0;

	tablaDePagina->punteroParaAlgoritmo = puntero;

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

		if(!pagina->usado&&pagina->modificado){

			encontrado = true;

			*nroPaginaAReemplazar = *acceso;

			*acceso = -1;

			break;
		}

		puntero++;

		modificarUso(*acceso,tablaDePagina->pid,false);
	}

	if(encontrado)
	puntero++;

	if(puntero>=list_size(tablaDePagina->listaParaAlgoritmo))
		puntero = 0;

	tablaDePagina->punteroParaAlgoritmo = puntero;

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

	case FIFO:
				agregarAccesoPorFIFO(tablaDePaginas,nroPagina);
		break;

	case LRU:

		agregarAccesoPorLRU(tablaDePaginas,nroPagina);

			break;

	case CLOCK_MODIFICADO:
		agregarAccesoPorCLOCKM(tablaDePaginas,nroPagina);
			break;
	}

	//if(estaHabilitadaLaTLB())
		//agregarAccesoPorFIFO(nroPagina,pid);
}

void inicializarPorAlgoritmo(tipoTablaPaginas* tablaDePagina){

	switch (datosMemoria->tipoDeAlgoritmoRAM) {

		case LRU:
			inicializarPorLRU(tablaDePagina->listaParaAlgoritmo,tablaDePagina->paginasPedidas);
				break;

		case CLOCK_MODIFICADO:
			inicializarPorLRU(tablaDePagina->listaParaAlgoritmo,minimoEntre(tablaDePagina->paginasPedidas,datosMemoria->configuracion->maximoDeMarcosPorProceso));
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

	if(estaHabilitadaLaTLB()){
	quitarDeTLB(nroPaginaAReemplazar,pid);
	//quitarAccesoTLB(nroPaginaAReemplazar,pid);
	}

	loguearColaDeAlgoritmo(tablaDePaginas);

	return dondeEstaEnRam;
}

void loguearColaDeAlgoritmo(tipoTablaPaginas* tabla){
	log_trace(datosMemoria->logDeAlgoritmos,"COLA DE ALGORITMO DEL PROCESO %d:", tabla->pid);

	int i;
	for (i = 0; i < list_size(tabla->listaParaAlgoritmo); ++i) {

		int aux = (int)(*(int*)list_get(tabla->listaParaAlgoritmo,i));
		log_trace(datosMemoria->logDeAlgoritmos,"%d", aux);

		if(datosMemoria->tipoDeAlgoritmoRAM == CLOCK_MODIFICADO){
			tipoPagina* pagina = list_get(tabla->frames, aux);

			log_trace(datosMemoria->logDeAlgoritmos,"M:%d | U:%d", pagina->modificado, pagina->usado);
		}
	}
	log_trace(datosMemoria->logDeAlgoritmos,"PUNTERO: %d", tabla->punteroParaAlgoritmo);
}

/*void agregarAccesoPorFIFO(int nroPagina,int pid){

	if(dondeEstaAccesoTLB(nroPagina,pid)<0){

	tipoAccesoAPaginaTLB* nuevoAcceso = malloc(sizeof(tipoAccesoAPaginaTLB));

	nuevoAcceso->nroPagina = nroPagina;

	nuevoAcceso->pid = pid;

	list_add(datosMemoria->colaFIFOTLB,nuevoAcceso);

	}
}*/

void agregarAccesoPorFIFO(tipoTablaPaginas* tablaDePAginas,int nroPagina){

	int var;
	for (var = 0; var < list_size(tablaDePAginas->listaParaAlgoritmo); ++var) {
		int* aux = list_get(tablaDePAginas->listaParaAlgoritmo,var);
		if(*aux==nroPagina)
			return;
	}

	int* nuevoAcceso = malloc(sizeof(int));

	*nuevoAcceso = nroPagina;

	list_add(tablaDePAginas->listaParaAlgoritmo,nuevoAcceso);
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

	if(!paginaYaAccedida)
		agregarEnHuecoCLOCKM(tablaDePaginas,nroPagina);

}

void agregarEnHuecoCLOCKM(tipoTablaPaginas* tablaDePaginas,int nroPagina){

	int* nuevoAcceso = malloc(sizeof(int));

	*nuevoAcceso = nroPagina;

	int var;

	int* acceso;

	for (var = 0 ; var < list_size(tablaDePaginas->listaParaAlgoritmo); ++var) {

		acceso = list_get(tablaDePaginas->listaParaAlgoritmo,var);

		if(*acceso<0){

			list_replace_and_destroy_element(tablaDePaginas->listaParaAlgoritmo,var,nuevoAcceso,free);

			break;
		}
	}
}

/*void quitarAccesoTLB(int nroPagina,int pid){

	int dondeEstaAcceso = dondeEstaAccesoTLB(nroPagina,pid);

	if(dondeEstaAcceso>=0)
		list_remove_and_destroy_element(datosMemoria->colaFIFOTLB,dondeEstaAcceso,free);
}*/

/*int dondeEstaAccesoTLB(int nroPagina,int pid){

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
}*/

int minimoEntre(int x,int y){

	if(x<y)
		return x;

	return y;
}








