/*
 * estructuras.c
 *
 *  Created on: 18/9/2015
 *      Author: utnso
 */
#include "estructuras.h"

void* serializarPCB(tipoPCB pcb,size_t* tamanioBloque){

	size_t tamanioRuta = strlen(pcb.ruta);//+sizeof(char);
	*tamanioBloque = sizeof(tipoPCB)-sizeof(char)+sizeof(size_t)+tamanioRuta;

	void* buffer = malloc(tamanioBloque);

	void* proximo = buffer;

	memcpy(proximo,&tamanioBloque,sizeof(size_t));  proximo+=sizeof(size_t);
	memcpy(proximo,&pcb.pid,sizeof(int));           proximo+=sizeof(int);
	memcpy(proximo,&pcb.estado,sizeof(char));  		proximo+=sizeof(char);
	memcpy(proximo,&pcb.insPointer,sizeof(int));  	proximo+=sizeof(int);
	memcpy(proximo,&pcb.ruta,tamanioRuta);

	return buffer;
}

tipoPCB recibirPCB(int socketEnviador){

	size_t tamanioBloque;

	printf("Recibiendo bloque...\n");

	void* bloque = recibirBloque(&tamanioBloque,socketEnviador);

	printf("Bloque recibido...\n");

	tipoPCB pcbRecibido = deserializarPCB(tamanioBloque,bloque);

	return pcbRecibido;
}

tipoPCB deserializarPCB(size_t tamanioBloque,void* bloque){

	tipoPCB pcbRecibido;

	void* proximo = bloque;

	size_t tamanioRuta = tamanioBloque-sizeof(tipoPCB)+sizeof(char);

	memcpy(&pcbRecibido.pid,proximo,sizeof(int)); 			proximo+=sizeof(int);
	memcpy(&pcbRecibido.estado,proximo,sizeof(char));		proximo+=sizeof(char);
	memcpy(&pcbRecibido.insPointer,proximo,sizeof(int));	proximo+=sizeof(int);
	memcpy(&pcbRecibido.ruta,proximo,tamanioRuta);

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

	deserializarIntYCadena(&instruccion.nroPagina,instruccion.texto,tamanioBloque,buffer+sizeof(char)+sizeof(int));

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
	size_t tamanioBloque = 4*sizeof(int)+sizeof(char)+tamanioTexto+sizeof(size_t);

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
