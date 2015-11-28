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
	char estado;
	int pid;
	int insPointer;
	char* ruta;
}__attribute__((packed)) tipoPCB;

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

typedef struct{
	int tamanio;
	char* datos;
}__attribute__((packed))tipoMensaje;




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


//------------------Serializacion-------------------//

tipoMensaje* crearMensaje(int unTamanio);
void destruirMensaje(tipoMensaje* unMensaje);


tipoMensaje* serializarInstruccion(tipoInstruccion* unaInstruccion);
tipoInstruccion* deserializarInstruccion(tipoMensaje* unMensaje);


tipoMensaje* serializarInstruccionPosta(tipoInstruccion* unaInstruccion);
tipoInstruccion* deserializarInstruccionPosta(tipoMensaje* unMensaje);


tipoMensaje* serializarRespuesta(tipoRespuesta* unaRespuesta);
tipoRespuesta* deserializarRespuesta(tipoMensaje* unMensaje);

tipoMensaje* serializarPCB(tipoPCB* unPCB);
tipoPCB* deserializarPCB(tipoMensaje* unMensaje);




#endif /* COMMONSDEASEDIO_ESTRUCTURAS_H_ */
