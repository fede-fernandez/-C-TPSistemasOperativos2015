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

//resultado de ejecucion
#define MANQUEADO 'm'
#define PERFECTO 'p'

//instrucciones
#define INICIAR 'i'
#define ESCRIBIR 'e'
#define LECTURA 'l'
#define FINALIZAR 'f'
#define ENTRADA-SALIDA 's'

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
}tipoInstruccion;

typedef struct{

	char respuesta;

	char* informacion;
}tipoRespuesta;



void* serializarPCB(tipoEstructuraPCB pcb);

tipoEstructuraPCB deserializarPCB(size_t tamanioBloque,void* bloque);

tipoEstructuraPCB recibirPCB(int socketPlanificador);

tipoInstruccion recibirInstruccion(int socketCpu);

tipoInstruccion deserializarInstruccion(size_t tamanioBloque,void* buffer);

void* serializarRespuesta(tipoRespuesta respuesta);

void* serializarInstruccion(tipoInstruccion instruccion);

#endif /* COMMONSDEASEDIO_ESTRUCTURAS_H_ */
