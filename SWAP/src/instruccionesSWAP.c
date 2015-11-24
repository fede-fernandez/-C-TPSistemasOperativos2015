/*
 * instruccionesSWAP.c
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */


#include "instruccionesSWAP.h"
#include "funcionesSWAP.h"

#include <commons/collections/list.h>


tipoRespuesta* ejecutarInstruccion(tipoInstruccion* instruccion,t_list* listaDeHuecos,tipoConfigSWAP* configuracion, t_log* logger){

	tipoRespuesta* respuesta;

	switch (instruccion->instruccion) {
		case INICIAR:
			respuesta = reservarEspacio(listaDeHuecos,instruccion->pid,instruccion->nroPagina,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,configuracion->nombreDeSWAP,configuracion->retardoDeCompactacion, logger);
			break;

		case LEER:
			respuesta = leerPagina(listaDeHuecos,instruccion->pid,instruccion->nroPagina,configuracion->tamanioDePagina,configuracion->nombreDeSWAP, logger);
			break;

		case ESCRIBIR:
			respuesta = escribirPagina(listaDeHuecos,instruccion->pid,instruccion->texto,instruccion->nroPagina,configuracion->tamanioDePagina,configuracion->nombreDeSWAP, logger);
			break;

		case FINALIZAR:
			respuesta = liberarEspacio(listaDeHuecos,instruccion->pid, configuracion->tamanioDePagina, logger);
			break;
	}

	return respuesta;

}
