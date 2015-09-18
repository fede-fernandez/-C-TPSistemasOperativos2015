/*
 * estructuras.c
 *
 *  Created on: 18/9/2015
 *      Author: utnso
 */
#include "estructuras.h"

void* serializarPCB(tipoEstructuraPCB pcb){

	size_t tamanioRuta = strlen(pcb.ruta);
	size_t tamanioBloque = 4*sizeof(int)+sizeof(char)+strlen(pcb.ruta);

	void* buffer = malloc(tamanioBloque);

	memcpy(buffer,&tamanioBloque,sizeof(size_t));
	memcpy(buffer+sizeof(size_t),&pcb.pid,sizeof(int));
	memcpy(buffer+sizeof(size_t)+sizeof(int),&pcb.estado,sizeof(char));
	memcpy(buffer+sizeof(size_t)+sizeof(int)+sizeof(char),&pcb.insPointer,sizeof(int));
	memcpy(buffer+sizeof(size_t)+2*sizeof(int)+sizeof(char),&pcb.ruta,tamanioRuta);

	return buffer;
}

tipoEstructuraPCB recibirPCB(int socketPlanificador){

	size_t tamanioBloque;

	recibirBloque(&tamanioBloque,socketPlanificador);

	void* bloque = malloc(tamanioBloque);

	recibirMensajeCompleto(socketPlanificador,bloque,tamanioBloque);

	tipoEstructuraPCB pcbRecibido = deserializarPCB(tamanioBloque,bloque);

	return pcbRecibido;
}

tipoEstructuraPCB deserializarPCB(size_t tamanioBloque,void* bloque){

	tipoEstructuraPCB pcbRecibido;

	memcpy(bloque,&pcbRecibido.pid,sizeof(int));tamanioBloque-=sizeof(int);
	memcpy(bloque+sizeof(int),&pcbRecibido.estado,sizeof(char));tamanioBloque-=sizeof(char);
	memcpy(bloque+sizeof(int)+sizeof(char),&pcbRecibido.insPointer,sizeof(int));tamanioBloque-=sizeof(int);
	memcpy(bloque+2*sizeof(int)+sizeof(char),&pcbRecibido.ruta,tamanioBloque);

	return pcbRecibido;
}

