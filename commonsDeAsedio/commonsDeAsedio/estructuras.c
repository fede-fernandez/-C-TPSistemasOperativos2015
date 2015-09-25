/*
 * estructuras.c
 *
 *  Created on: 18/9/2015
 *      Author: utnso
 */
#include "estructuras.h"

void* serializarPCB(tipoPCB pcb){

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

tipoPCB recibirPCB(int socketPlanificador){

	size_t tamanioBloque;

	recibirBloque(&tamanioBloque,socketPlanificador);

	void* bloque = malloc(tamanioBloque);

	recibirMensajeCompleto(socketPlanificador,bloque,tamanioBloque);

	tipoPCB pcbRecibido = deserializarPCB(tamanioBloque,bloque);

	return pcbRecibido;
}

tipoPCB deserializarPCB(size_t tamanioBloque,void* bloque){

	tipoPCB pcbRecibido;

	memcpy(bloque,&pcbRecibido.pid,sizeof(int));tamanioBloque-=sizeof(int);
	memcpy(bloque+sizeof(int),&pcbRecibido.estado,sizeof(char));tamanioBloque-=sizeof(char);
	memcpy(bloque+sizeof(int)+sizeof(char),&pcbRecibido.insPointer,sizeof(int));tamanioBloque-=sizeof(int);
	memcpy(bloque+2*sizeof(int)+sizeof(char),&pcbRecibido.ruta,tamanioBloque);

	return pcbRecibido;
}

tipoInstruccion recibirInstruccion(int socketCpu){

	size_t tamanioBloque;

	recibirBloque(&tamanioBloque,socketCpu);

	void* buffer = malloc(tamanioBloque);

	tipoInstruccion instruccionRecibida = deserializarInstruccion(tamanioBloque, buffer);

	return instruccionRecibida;
}

tipoInstruccion deserializarInstruccion(size_t tamanioBloque,void* buffer){

	tipoInstruccion instruccion;

	memcpy(buffer,&instruccion.pid,sizeof(int));tamanioBloque-=sizeof(int);

	memcpy(buffer+sizeof(int),&instruccion.instruccion,sizeof(char));tamanioBloque-=sizeof(char);

	deserializarIntYCadena(&instruccion.nroPagina,&instruccion.texto,tamanioBloque,buffer+sizeof(char)+sizeof(int));

	return instruccion;
}

void* serializarRespuesta(tipoRespuesta respuesta){

	size_t tamanioInformacion = strlen(respuesta.informacion);

	size_t tamanioBloque = tamanioInformacion+sizeof(char);

	void* buffer = malloc(tamanioBloque+sizeof(int));

	memcpy(buffer,&tamanioBloque,sizeof(size_t));
	memcpy(buffer+sizeof(size_t),&respuesta.respuesta,sizeof(char));
	memcpy(buffer+sizeof(size_t)+sizeof(char),&tamanioInformacion,sizeof(size_t));
	memcpy(buffer+2*sizeof(size_t)+sizeof(char),&respuesta.informacion,tamanioInformacion);

	return buffer;
}
void* serializarInstruccion(tipoInstruccion instruccion){

	size_t tamanioTexto = strlen(instruccion.texto);
	size_t tamanioBloque = 4*sizeof(int)+sizeof(char)+tamanioTexto;

		void* buffer = malloc(tamanioBloque);

		memcpy(buffer,&tamanioBloque,sizeof(size_t));
		memcpy(buffer+sizeof(size_t),&instruccion.pid,sizeof(int));
		memcpy(buffer+sizeof(size_t)+sizeof(int),&instruccion.instruccion,sizeof(char));
		memcpy(buffer+sizeof(size_t)+sizeof(int)+sizeof(char),&instruccion.nroPagina,sizeof(int));
		memcpy(buffer+sizeof(size_t)+2*sizeof(int)+sizeof(char),&instruccion.texto,tamanioTexto);

		return buffer;

}

tipoRespuesta deserializarRespuesta(size_t tamanioBloque,void* buffer){

	size_t tamanioInformacion = tamanioBloque-sizeof(char);

	tipoRespuesta respuesta;

	memcpy(&respuesta.respuesta,buffer,sizeof(char));
	memcpy(&respuesta.informacion,buffer+sizeof(char),tamanioInformacion);

	return respuesta;
}

/*******************Funciones para tipoRespuesta*************************/

tipoRespuesta* crearTipoRespuesta(char respuesta, char* informacion){
	tipoRespuesta* aux = malloc(sizeof(tipoRespuesta));

	aux->respuesta = respuesta;
	aux->informacion = informacion;

	return aux;
}

void destruirTipoRespuesta(tipoRespuesta* respuesta){
	free(respuesta->informacion);
	free(respuesta);
}
