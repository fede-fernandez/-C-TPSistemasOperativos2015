/*
 * estructuras.h
 *
 *  Created on: 18/9/2015
 *      Author: utnso
 */

#ifndef COMMONSDEASEDIO_ESTRUCTURAS_H_
#define COMMONSDEASEDIO_ESTRUCTURAS_H_

#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cliente-servidor.h"


typedef struct{
	int pid;
	int insPointer;
	char estado;
	char* ruta;
} tipoEstructuraPCB;

typedef struct{
	char instruccion;

	int pid;

	int nroPagina;

	char* texto;
}tipoInstruccionCpu;

typedef struct{

	char respuesta;

	char* informacion;
}tipoRespuestaCpu;



void* serializarPCB(tipoEstructuraPCB pcb);

tipoEstructuraPCB deserializarPCB(size_t tamanioBloque,void* bloque);

tipoEstructuraPCB recibirPCB(int socketPlanificador);


tipoInstruccionCpu recibirInstruccionCpu(int socketCpu);

tipoInstruccionCpu deserializarInstruccionCpu(size_t tamanioBloque,void* buffer);

void* serializarRespuestaCpu(tipoRespuestaCpu respuesta);


#endif /* COMMONSDEASEDIO_ESTRUCTURAS_H_ */
