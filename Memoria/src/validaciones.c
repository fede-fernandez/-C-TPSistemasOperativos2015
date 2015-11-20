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

return cantidadDeMarcosUsadosPor(pid)>=datosMemoria->configuracion->maximoDeMarcosPorProceso;
}

int cantidadDeMarcosUsadosPor(int pid){

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	return tablaDePagina->paginasAsignadas;
}

bool noUsaMarcos(int pid){

	return cantidadDeMarcosUsadosPor(pid)==0;
}


bool TLBLlena(){
	return list_size(datosMemoria->listaTLB)>=datosMemoria->configuracion->entradasDeTLB;
}

bool RAMLlena(){

	int posicionDeHueco = buscarHuecoRAM();

	return posicionDeHueco<0;//(list_size(datosMemoria->listaRAM)>=datosMemoria->configuracion->cantidadDeMarcos);
}


bool tamanioDePaginaMayorAlSoportado(char* texto){

	return string_length(texto)>=datosMemoria->configuracion->tamanioDeMarco;
}

bool numeroDePaginaIncorrecto(int nroPagina,int pid){

	tipoTablaPaginas* tablaDePagina = traerTablaDePaginas(pid);

	return tablaDePagina->paginasPedidas<=nroPagina;
}
