/*
 * serializacion.h
 *
 *  Created on: 25/11/2015
 *      Author: utnso
 */

#ifndef COMMONSDEASEDIO_SERIALIZACION_H_
#define COMMONSDEASEDIO_SERIALIZACION_H_


#include <stdio.h>
#include <stdlib.h>

#include <commons/string.h>
#include "estructuras.h"

typedef struct{
	int tamanio;
	char* datos;
}__attribute__((packed))tipoMensaje;


tipoMensaje* crearMensaje(int unTamanio);
void destruirMensaje(tipoMensaje* unMensaje);


tipoMensaje* serializarInstruccion(tipoInstruccion* unaInstruccion);
tipoInstruccion* deserializarInstruccion(tipoMensaje* unMensaje);


tipoMensaje* serializarInstruccionPosta(tipoInstruccion* unaInstruccion);
tipoInstruccion* deserializarInstruccionPosta(tipoMensaje* unMensaje);


tipoMensaje* serializarRespuesta(tipoRespuesta* unaRespuesta);
tipoRespuesta* deserializarRespuesta(tipoMensaje* unMensaje);


#endif /* COMMONSDEASEDIO_SERIALIZACION_H_ */
