/*
 * algoritmos.c
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#include "algoritmos.h"

void setearParaAlgoritmos(/*tipoEstructuraMemoria* datos*/){
	//datosMemoria = datos;

	datosMemoria->tipoDeAlgoritmoRAM = LRU;//Hardcodeado hasta que no se

	datosMemoria->tipoDeAlgoritmoTLB = FIFO;//agregue en el archivo de config

}

int cualReemplazarTLB(){


	int cualReemplazar;

switch (datosMemoria->tipoDeAlgoritmoTLB) {

	case FIFO:
		cualReemplazar = cualReemplazarTLBFIFO();
		break;

	case LRU:
		cualReemplazar = cualReemplazarTLBLRU();
		break;

	case CLOCK_MODIFICADO:
		cualReemplazar = cualReemplazarTLBCLOCKM();
			break;
}

	return cualReemplazar;
}

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
		cualReemplazar = cualReemplazarRAMCLOCKM();
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

int cualReemplazarRAMCLOCKM(){

return 0;
}

int cualReemplazarTLBFIFO(){

	return 0;
}

int cualReemplazarTLBLRU(){

	return 0;
}

int cualReemplazarTLBCLOCKM(){

	return 0;
}

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

	if(estaHabilitadaLaTLB())
	quitarDeTLB(nroPaginaAReemplazar,pid);

	return dondeEstaEnRam;

}

void agregarAccesoPorFIFO(tipoTablaPaginas* tablaDePaginas,int nroPagina){

	int* nuevoAcceso = malloc(sizeof(int));

	*nuevoAcceso = nroPagina;

	list_add(tablaDePaginas->listaParaAlgoritmo,nuevoAcceso);

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





