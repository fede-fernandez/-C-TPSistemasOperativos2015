/*
 * estructuras.c
 *
 *  Created on: 18/9/2015
 *      Author: utnso
 */
#include "estructuras.h"

void* serializarPCB(tipoPCB pcb){

	size_t tamanioRuta = strlen(pcb.ruta)+sizeof(char);

	size_t tamanioBloque = 2*sizeof(int)+sizeof(char)+tamanioRuta+sizeof(size_t);

	void* buffer = malloc(tamanioBloque+sizeof(size_t));

	void* proximo = buffer;

	memcpy(proximo,&tamanioBloque,sizeof(size_t));  proximo+=sizeof(size_t);
	memcpy(proximo,&(pcb.pid),sizeof(int));           proximo+=sizeof(int);
	memcpy(proximo,&(pcb.estado),sizeof(char));  		proximo+=sizeof(char);
	memcpy(proximo,&(pcb.insPointer),sizeof(int));  	proximo+=sizeof(int);
	memcpy(proximo,&tamanioRuta,sizeof(size_t));  proximo+=sizeof(size_t);
	memcpy(proximo,pcb.ruta,tamanioRuta);

	return buffer;
}

void imprimirBufferPCB(void* buffer){

	void* proximo = buffer;

	printf("--------BUFFER-----------\n");
	printf("PCB:\n");
	printf("PID: %d\n",*((int*)proximo));     proximo+=sizeof(int);
	printf("STATE: %c\n",*((char*)proximo));	 proximo+=sizeof(char);
	printf("IP: %d\n",*((int*)proximo));		 proximo+=sizeof(int)+sizeof(size_t);
	printf("DIR: %s\n",(char*)proximo);
	printf("-------------------------\n");

}

tipoPCB* recibirPCB(int socketEnviador){

	size_t tamanioBloque;

	void* bloque = recibirBloque(&tamanioBloque,socketEnviador);

	tipoPCB pcbRecibido;

	imprimirBufferPCB(bloque);

	deserializarPCB(bloque,&pcbRecibido);

	free(bloque);

	return &pcbRecibido;
}

void enviarPCB(int socketCliente,tipoPCB pcb){

	size_t tamanioBloque = 2*(sizeof(char)+sizeof(int)+sizeof(size_t))+strlen(pcb.ruta);

	void* bloque = serializarPCB(pcb);

	enviarMensaje(socketCliente,bloque,tamanioBloque);

	free(bloque);
}

void deserializarPCB(void* bloque, tipoPCB* pcbRecibido){

	void* proximo = bloque;

	size_t tamanioRuta;// = tamanioBloque-2*sizeof(int)-sizeof(char);

	memcpy(&(pcbRecibido->pid),proximo,sizeof(int));         proximo+=sizeof(int);
	memcpy(&(pcbRecibido->estado),proximo,sizeof(char));	 proximo+=sizeof(char);
	memcpy(&(pcbRecibido->insPointer),proximo,sizeof(int));	 proximo+=sizeof(int);
	memcpy(&tamanioRuta,proximo,sizeof(size_t));			 proximo+=sizeof(size_t);
	//memcpy(pcbRecibido->ruta,proximo,tamanioRuta);

	pcbRecibido->ruta = proximo;
}

tipoInstruccion* recibirInstruccion(int socketEnviador){

	size_t tamanioBloque;

	recibirBloque(&tamanioBloque,socketEnviador);

	void* buffer = malloc(tamanioBloque);

	tipoInstruccion instruccionRecibida;

	deserializarInstruccion(buffer,&instruccionRecibida);

	return &instruccionRecibida;
}

void deserializarInstruccion(void* buffer,tipoInstruccion* instruccion){

	void* proximo = buffer;

	size_t tamanioTexto;

	memcpy(&(instruccion->pid),proximo,sizeof(int));         proximo+=sizeof(int);
	memcpy(&(instruccion->instruccion),proximo,sizeof(char));	 proximo+=sizeof(char);
	memcpy(&(instruccion->nroPagina),proximo,sizeof(int));	 proximo+=sizeof(int);
	memcpy(&tamanioTexto,proximo,sizeof(size_t));			 proximo+=sizeof(size_t);
	//memcpy(instruccion->texto,proximo,tamanioTexto);

	instruccion->texto = proximo;

}

void* serializarRespuesta(tipoRespuesta respuesta){

	size_t tamanioInformacion = strlen(respuesta.informacion)+sizeof(char);

	size_t tamanioBloque = tamanioInformacion+sizeof(char)+sizeof(size_t);

	void* buffer = malloc(tamanioBloque+sizeof(size_t));

	void* proximo = buffer;

	memcpy(proximo,&tamanioBloque,sizeof(size_t));          		proximo+=sizeof(size_t);
	memcpy(proximo,&(respuesta.respuesta),sizeof(char));			proximo+=sizeof(char);
	memcpy(proximo,&tamanioInformacion,sizeof(size_t));          	proximo+=sizeof(size_t);
	memcpy(proximo,respuesta.informacion,tamanioInformacion);

	return buffer;
}

void* serializarInstruccion(tipoInstruccion instruccion){

	size_t tamanioTexto = strlen(instruccion.texto)+sizeof(char);
	size_t tamanioBloque = 2*sizeof(int)+sizeof(char)+tamanioTexto+sizeof(size_t);

		void* buffer = malloc(tamanioBloque+sizeof(size_t));

		void* proximo = buffer;

		memcpy(proximo,&tamanioBloque,sizeof(size_t));         	proximo+=sizeof(size_t);
		memcpy(proximo,&(instruccion.pid),sizeof(int));			proximo+=sizeof(int);
		memcpy(proximo,&(instruccion.instruccion),sizeof(char));	proximo+=sizeof(char);
		memcpy(proximo,&(instruccion.nroPagina),sizeof(int));		proximo+=sizeof(int);
		memcpy(proximo,&tamanioTexto,sizeof(size_t));         	proximo+=sizeof(size_t);
		memcpy(proximo,instruccion.texto,tamanioTexto);

		return buffer;
}

void enviarInstruccion(int socketCliente,tipoInstruccion instruccion){

	size_t tamanioBloque = 2*(sizeof(char)+sizeof(int)+sizeof(size_t))+strlen(instruccion.texto);

	void* bloque = serializarInstruccion(instruccion);

	enviarMensaje(socketCliente,bloque,tamanioBloque);

	free(bloque);
}


void deserializarRespuesta(void* buffer,tipoRespuesta* respuesta){

	size_t tamanioInformacion;

	void* proximo = buffer;

	memcpy(&(respuesta->respuesta),proximo,sizeof(char));		proximo+=sizeof(char);
	memcpy(&tamanioInformacion,proximo,sizeof(char));			proximo+=sizeof(size_t);
	//memcpy(respuesta->informacion,proximo,tamanioInformacion);

	respuesta->informacion = proximo;

}

void enviarRespuesta(int socketCliente,tipoRespuesta respuesta){

	size_t tamanioBloque = 2*sizeof(size_t)+sizeof(char)+strlen(respuesta.informacion);

	void* buffer = serializarRespuesta(respuesta);

	enviarMensaje(socketCliente,buffer,tamanioBloque);

	free(buffer);
}

tipoRespuesta* recibirRespuesta(int socketEnviador){

	size_t tamanioBloque;

	void* buffer = recibirBloque(&tamanioBloque,socketEnviador);

	tipoRespuesta respuesta;

	deserializarRespuesta(buffer,&respuesta);

	free(buffer);

	return &respuesta;

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

