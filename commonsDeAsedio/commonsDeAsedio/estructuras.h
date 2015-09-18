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


void* serializarPCB(tipoEstructuraPCB pcb);

tipoEstructuraPCB deserializarPCB(size_t tamanioBloque,void* bloque);

tipoEstructuraPCB recibirPCB(int socketPlanificador);

#endif /* COMMONSDEASEDIO_ESTRUCTURAS_H_ */
