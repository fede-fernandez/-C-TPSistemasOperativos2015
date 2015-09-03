/*
 * cliente-servidor.c
 *
 *  Created on: 1/6/2015
 *      Author: utnso
 */
#include "cliente-servidor.h"




void validarError(int validar, char* mensaje){

	if(validar < 0){

		perror(mensaje);

		exit(-1);
	}
}

int crearSocket(){

	int socketNuevo;

	socketNuevo = socket(AF_INET, SOCK_STREAM, 0);

	validarError(socketNuevo,"Error al crear el socket\n");

	return socketNuevo;
}

void asociarAPuerto(int socket, int puerto){

	obviarErrorDeIpEnUso(socket);

	struct sockaddr_in servidor;
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(puerto);
	servidor.sin_addr.s_addr = INADDR_ANY;

	memset(&(servidor.sin_zero), '\0', 8);

	int conexion = bind(socket,(struct sockaddr*) &servidor,sizeof(servidor));

	validarError(conexion, "No se puede enlazar el puerto: direccion ya esta en uso\n");
}

void liberarSocket(int socket){
	//cierro el socket
	close(socket);
}

void obviarErrorDeIpEnUso(int socket){
	static int valorDeOpcion = 1;
	//libero el puerto cuando se cierra el socket
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &valorDeOpcion, sizeof(valorDeOpcion));

}

void escucharConexiones(int socket, int maximasConexionesEntrantes){

		int escuchar = listen(socket, maximasConexionesEntrantes);

		validarError(escuchar, "Error al escuchar conecciones\n");
	}
void conectarAServidor(int socket,char* direccionIp,int puerto){

	struct sockaddr_in informacionSocket;

	informacionSocket.sin_family = AF_INET;
	informacionSocket.sin_addr.s_addr = inet_addr(direccionIp);
	informacionSocket.sin_port = htons(puerto);

	int conexion = connect(socket, (struct sockaddr*) &informacionSocket, sizeof(informacionSocket));

	validarError(conexion,"Error al conectar con servidor");

}

int enviarMensaje(int socketCliente, void* buffer,int tamanioBuffer){//Aca hay un problema con sizeof() que siempre devuelve 4

	//int tamanio=sizeof(buffer);

	int enviador = send(socketCliente, buffer,tamanioBuffer, 0);//Envia mensaje

	validarError(enviador, "Error al enviar el mensaje");

	return enviador;
}


int recibirMensaje(int socketCliente, void* buffer,int tamanioBuffer){

	//int tamanio=sizeof(buffer);

	int mensajeRecibido = recv(socketCliente, buffer,tamanioBuffer,0);//Recibe mensaje

	validarError(mensajeRecibido,"Error de recepcion de datos");

	return mensajeRecibido;

}

int recibirMensajeCompleto(int socketCliente, void* buffer,int tamanioBuffer){

	//int tamanio=sizeof(buffer);

	int mensajeRecibido = recv(socketCliente, buffer,tamanioBuffer,MSG_WAITALL);//Recibe mensaje

	validarError(mensajeRecibido,"Error de recepcion de datos");

	return mensajeRecibido;

}

int crearSocketParaAceptarSolicitudes(int socketServidor){
	/*int socketAceptarSolicitud = accept(socket, NULL, 0);
	validarError(socketAceptarSolicitud, "Error al aceptar solicitudes\n");*/ //forma de Internet

	//creo un cliente
	struct sockaddr_in cliente;

	unsigned int tamanioCliente = sizeof(cliente);

	//acepto solicitud
	int socketAceptarSolicitud = accept(socketServidor,(struct sockaddr*) &cliente,&tamanioCliente);

	validarError(socketAceptarSolicitud, "Error al aceptar solicitudes\n");

	return socketAceptarSolicitud;
}

void* recibirBloque(size_t* tamanioBloque,int socketEnviador){

	size_t tamanio;

	recibirMensajeCompleto(socketEnviador,&tamanio,sizeof(size_t));

	tamanio -= sizeof(size_t);

	void* buffer = malloc(tamanio);

	recibirMensajeCompleto(socketEnviador,buffer,tamanio);

	*tamanioBloque = tamanio;

	return buffer;
}

void serializarIntYCadena(int entero,char* cadena,void* bloque){

	size_t longitudBloque = strlen(cadena)+sizeof(int)+sizeof(size_t);

	memcpy(bloque,&longitudBloque,sizeof(size_t));

	memcpy(bloque+sizeof(size_t),&entero,sizeof(int));

	memcpy(bloque+sizeof(int)+sizeof(size_t),cadena,strlen(cadena));

}

void deserializarIntYCadena(int* entero,char* cadena,int tamanioBloque,void* bloque){

	size_t longitudCadena = tamanioBloque-sizeof(int);

	memcpy(entero,bloque,sizeof(int));

	memcpy(cadena,bloque+sizeof(int),longitudCadena);

	string_append(&cadena,"\0");

}



/*void tratarPeticiones(int socketServidor,int* listaDeDescriptores,int longitud){

	fd_set listaDeSockets=crearListaDeSockets(socketServidor,listaDeDescriptores,longitud);

	fd_set listaParaLeer = listaDeSockets;

	fd_set listaParaEscribir=listaDeSockets;

	fd_set listaDeInterrupciones=listaDeSockets;

	select(maximoDe(listaDeDescriptores,longitud) + 1,&listaParaLeer,&listaParaEscribir,&listaDeInterrupciones,0);

	//tratarLectura(listaParaLeer);

	//tratarInterrupciones(listaDeInterrupciones);

	//tratarEscritura(listaParaEscribir);

}*/


