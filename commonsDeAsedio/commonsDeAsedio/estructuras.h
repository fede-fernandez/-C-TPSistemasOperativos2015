/*
 * estructuras.h
 *
 *  Created on: 3/10/2015
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
#define FINALIZAR 		'f'
#define ESCRIBIR 		'e'
#define LEER 			'l'
#define INICIAR 		'i'
#define ENTRADA_SALIDA 	's'
#define FINALIZAR_PROCESO 'h'//Esto lo agrego para que los procesos terminen de forma ordenada

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


tipoPCB* recibirPCB(int socketEnviador);

void enviarPCB(int socketCliente,tipoPCB* pcb);

void imprimirPCB(tipoPCB* PCB);

tipoInstruccion* recibirInstruccion(int socketEnviador);

void enviarInstruccion(int socketCliente,tipoInstruccion* instruccion);

tipoRespuesta* recibirRespuesta(int socketEnviador);

void enviarRespuesta(int socketCliente,tipoRespuesta* respuesta);

//-------------------LEONEGRO----------------------------------------

//ronan: esto lo hice yo lpm, dejen de robar mi trabajo

tipoInstruccion* crearTipoInstruccion(int pid,char instruccion,int nroPagina,char* texto);

tipoRespuesta* crearTipoRespuesta(char respuesta, char* informacion);

void destruirTipoRespuesta(tipoRespuesta* respuesta);

void destruirTipoInstruccion(tipoInstruccion* instruccion);


#endif /* COMMONSDEASEDIO_ESTRUCTURAS_H_ */
