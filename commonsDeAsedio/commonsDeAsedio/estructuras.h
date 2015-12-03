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

//colores
#define NEGRO "\033[1m\033[30m"
#define ROJO "\033[1m\033[31m"
#define VERDE "\033[1m\033[32m"
#define AMARILLO "\033[1m\033[33m"
#define AZUL "\033[1m\033[34m"
#define ROSA "\033[1m\033[35m"
#define CELESTE "\033[1m\033[36m"
#define BLANCO "\033[1m\033[37m"
#define FINDETEXTO "\033[0m"


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
}__attribute__((packed))tipoInstruccion;

typedef struct{
	char respuesta;
	char* informacion;
}__attribute__((packed))tipoRespuesta;




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
