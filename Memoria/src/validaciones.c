/*
 * validaciones.c
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */
#include "validaciones.h"

void setearParaValidaciones(tipoEstructuraMemoria* datos){
	datosMemoria = datos;
}

bool estaHabilitadaLaTLB(){

	return string_equals_ignore_case(datosMemoria->configuracion->TLBHabilitada,"SI");
}

bool procesoExiste(int pid){

	return buscarTabla(pid)>=0;
}

bool excedeMaximoDeMarcos(int pid){

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	return tablaDePagina->paginasAsignadas>=datosMemoria->configuracion->maximoDeMarcosPorProceso;
}

bool TLBLlena(){
	return list_size(datosMemoria->listaTLB)>=datosMemoria->configuracion->entradasDeTLB;
}

bool RAMLlena(){
	return (list_size(datosMemoria->listaRAM)>=datosMemoria->configuracion->cantidadDeMarcos);
}

bool tamanioDePaginaMayorAlSoportado(char* texto){

	return string_length(texto)>=datosMemoria->configuracion->tamanioDeMarco;
}

bool numeroDePaginaIncorrecto(int nroPagina,int pid){

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	return tablaDePagina->paginasPedidas<=nroPagina;
}
