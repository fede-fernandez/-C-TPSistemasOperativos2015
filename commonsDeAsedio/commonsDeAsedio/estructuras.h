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
#include <commons/string.h>


//resultado de ejecucion
#define MANQUEADO 'm'
#define PERFECTO 'p'

//instrucciones
#define INICIAR 'i'
#define ESCRIBIR 'e'
#define LECTURA 'l'
#define FINALIZAR 'f'
#define ENTRADA_SALIDA 's'

typedef struct{
	int pid;
	int insPointer;
	char estado;
	char* ruta;
} tipoPCB;

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

/***********************PRUEBAS************************/
/************ESTRUCTURAS***************/
typedef struct{
	int numero;
	char* cadena;
}tipoEstructura;

void imprimirEstructura(tipoEstructura estructura);
void* serializar(tipoEstructura estructura);
void desSerializar(void* buffer, tipoEstructura* estructura);
/****************************************/



void* serializarPCB(tipoPCB pcb,size_t* tamanio);

void deserializarPCB(void* bloque, tipoPCB* pcbrecibido);

//tipoPCB recibirPCB(int socketPlanificador);

tipoInstruccion recibirInstruccion(int socketCpu);

tipoInstruccion deserializarInstruccion(size_t tamanioBloque,void* buffer);

void* serializarRespuesta(tipoRespuesta respuesta,size_t* tamanio);

void* serializarInstruccion(tipoInstruccion instruccion,size_t* tamanio);

tipoRespuesta deserializarRespuesta(size_t tamanioBloque,void* buffer);

tipoRespuesta* crearTipoRespuesta(char respuesta, char* informacion);

void destruirTipoRespuesta(tipoRespuesta* respuesta);

#endif /* COMMONSDEASEDIO_ESTRUCTURAS_H_ */
