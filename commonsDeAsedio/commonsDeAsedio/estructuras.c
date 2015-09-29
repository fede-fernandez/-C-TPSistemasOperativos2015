/*
 * estructuras.c
 *
 *  Created on: 18/9/2015
 *      Author: utnso
 */
#include "estructuras.h"

void* serializarPCB(tipoPCB pcb,size_t* tamanio){

	size_t tamanioRuta = strlen(pcb.ruta)+sizeof(char);

	size_t tamanioBloque = 2*sizeof(int)+sizeof(char)+sizeof(size_t)+tamanioRuta;

	void* buffer = malloc(tamanioBloque);

	void* proximo = buffer;

	memcpy(proximo,&tamanioBloque,sizeof(size_t));  proximo+=sizeof(size_t);
	memcpy(proximo,&pcb.pid,sizeof(int));           proximo+=sizeof(int);
	memcpy(proximo,&pcb.estado,sizeof(char));  		proximo+=sizeof(char);
	memcpy(proximo,&pcb.insPointer,sizeof(int));  	proximo+=sizeof(int);
	memcpy(proximo,pcb.ruta,tamanioRuta);

	*tamanio = tamanioBloque;

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

	size_t tamanioRuta = tamanioBloque-2*sizeof(int)-sizeof(char);

	memcpy(&pcbRecibido.pid,proximo,sizeof(int)); 			proximo+=sizeof(int);
	memcpy(&pcbRecibido.estado,proximo,sizeof(char));		proximo+=sizeof(char);
	memcpy(&pcbRecibido.insPointer,proximo,sizeof(int));	proximo+=sizeof(int);
	memcpy(pcbRecibido.ruta,proximo,tamanioRuta);

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

	size_t tamanioIntYCadena = tamanioBloque-sizeof(char)-sizeof(int);

	void* proximo = buffer;

	memcpy(&instruccion.pid,proximo,sizeof(int));   		proximo+=sizeof(int);

	memcpy(&instruccion.instruccion,proximo,sizeof(char));  proximo+=sizeof(char);

	deserializarIntYCadena(&instruccion.nroPagina,instruccion.texto,tamanioIntYCadena,proximo);

	return instruccion;
}

void* serializarRespuesta(tipoRespuesta respuesta,size_t* tamanioRespuesta){

	size_t tamanioInformacion = strlen(respuesta.informacion)+sizeof(char);

	size_t tamanioBloque = tamanioInformacion+sizeof(char)+sizeof(size_t);

	void* buffer = malloc(tamanioBloque);

	void* proximo = buffer;

	memcpy(proximo,&tamanioBloque,sizeof(size_t));          		proximo+=sizeof(size_t);
	memcpy(proximo,&respuesta.respuesta,sizeof(char));				proximo+=sizeof(char);
	memcpy(proximo,respuesta.informacion,tamanioInformacion);

	*tamanioRespuesta = tamanioBloque;

	return buffer;
}
void* serializarInstruccion(tipoInstruccion instruccion,size_t* tamanioInstruccion){

	size_t tamanioTexto = strlen(instruccion.texto)+sizeof(char);
	size_t tamanioBloque = 2*sizeof(int)+sizeof(char)+tamanioTexto+sizeof(size_t);

		void* buffer = malloc(tamanioBloque);

		void* proximo = buffer;

		memcpy(proximo,&tamanioBloque,sizeof(size_t));         	proximo+=sizeof(size_t);
		memcpy(proximo,&instruccion.pid,sizeof(int));			proximo+=sizeof(int);
		memcpy(proximo,&instruccion.instruccion,sizeof(char));	proximo+=sizeof(char);
		memcpy(proximo,&instruccion.nroPagina,sizeof(int));		proximo+=sizeof(int);
		memcpy(proximo,instruccion.texto,tamanioTexto);

		*tamanioInstruccion = tamanioBloque;

		return buffer;

}

tipoRespuesta deserializarRespuesta(size_t tamanioBloque,void* buffer){

	size_t tamanioInformacion = tamanioBloque-sizeof(char);

	tipoRespuesta respuesta;

	memcpy(&respuesta.respuesta,buffer,sizeof(char));
	memcpy(respuesta.informacion,buffer+sizeof(char),tamanioInformacion);

	return respuesta;
}

/*******************Funciones para tipoRespuesta*************************/

tipoRespuesta* crearTipoRespuesta(char respuesta, char* informacion){//esto es al dope
	tipoRespuesta* aux = malloc(sizeof(tipoRespuesta));

	aux->respuesta = respuesta;
	aux->informacion = informacion;

	return aux;
}

void destruirTipoRespuesta(tipoRespuesta* respuesta){
	free(respuesta->informacion);
	free(respuesta);
}
