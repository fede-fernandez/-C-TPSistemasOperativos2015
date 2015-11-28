/*
 * serializacion.c
 *
 *  Created on: 25/11/2015
 *      Author: utnso
 */

#include "serializacion.h"


tipoMensaje* crearMensaje(int unTamanio){
	tipoMensaje* unMensaje = malloc(sizeof(tipoMensaje));

	unMensaje->tamanio = unTamanio;
	unMensaje->datos = malloc(unTamanio);

	return unMensaje;
}

void destruirMensaje(tipoMensaje* unMensaje){
	free(unMensaje->datos);
	free(unMensaje);
}


tipoMensaje* serializarInstruccion(tipoInstruccion* unaInstruccion){

	int offset = 0;
	tipoMensaje* unMensaje = crearMensaje(sizeof(char) + 2*sizeof(int) + string_length(unaInstruccion->texto) + 1);

	memcpy(unMensaje->datos + offset, &unaInstruccion->instruccion, sizeof(char));
	offset += sizeof(char);

	memcpy(unMensaje->datos + offset, &unaInstruccion->pid, sizeof(int));
	offset += sizeof(int);

	memcpy(unMensaje->datos + offset, &unaInstruccion->nroPagina, sizeof(int));
	offset += sizeof(int);

	memcpy(unMensaje->datos + offset, unaInstruccion->texto, string_length(unaInstruccion->texto) + 1);


	return unMensaje;
}

tipoInstruccion* deserializarInstruccion(tipoMensaje* unMensaje){

	int offset = 0;
	tipoInstruccion* unaInstruccion = malloc(sizeof(tipoInstruccion));

	memcpy(&unaInstruccion->instruccion, unMensaje->datos + offset, sizeof(char));
	offset += sizeof(char);

	memcpy(&unaInstruccion->pid, unMensaje->datos + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&unaInstruccion->pid, unMensaje->datos + offset, sizeof(int));
	offset += sizeof(int);

	unaInstruccion->texto = string_duplicate(unMensaje->datos + offset);


	return unaInstruccion;
}


tipoMensaje* serializarInstruccionPosta(tipoInstruccion* unaInstruccion){

	int offset = 0;
	tipoMensaje* unMensaje = crearMensaje(sizeof(char) + 2*sizeof(int) + string_length(unaInstruccion->texto) + 1);

	memcpy(unMensaje->datos + offset, unaInstruccion, sizeof(char) + 2*sizeof(int));
	offset += sizeof(char) + 2*sizeof(int);

	memcpy(unMensaje->datos + offset, unaInstruccion->texto, string_length(unaInstruccion->texto) + 1);


	return unMensaje;
}

tipoInstruccion* deserializarInstruccionPosta(tipoMensaje* unMensaje){

	int offset = 0;
	tipoInstruccion* unaInstruccion = malloc(sizeof(tipoInstruccion));

	memcpy(unaInstruccion + offset, unMensaje->datos, sizeof(char) + 2*sizeof(int));
	offset += sizeof(char) + 2*sizeof(int);

	unaInstruccion->texto = string_duplicate(unMensaje->datos + offset);


	return unaInstruccion;
}



tipoMensaje* serializarRespuesta(tipoRespuesta* unaRespuesta){

	int offset = 0;
	tipoMensaje* unMensaje = crearMensaje(sizeof(char) + string_length(unaRespuesta->informacion) + 1);

	memcpy(unMensaje->datos + offset, &unaRespuesta->respuesta, sizeof(char));
	offset += sizeof(char);

	memcpy(unMensaje->datos + offset, unaRespuesta->informacion, string_length(unaRespuesta->informacion) + 1);


	return unMensaje;
}

tipoRespuesta* deserializarRespuesta(tipoMensaje* unMensaje){

	int offset = 0;
	tipoRespuesta* unaRespuesta = malloc(sizeof(tipoRespuesta));

	memcpy(&unaRespuesta->respuesta, unMensaje->datos + offset, sizeof(char));
	offset += sizeof(char);

	unaRespuesta->informacion = string_duplicate(unMensaje->datos + offset);


	return unaRespuesta;
}
