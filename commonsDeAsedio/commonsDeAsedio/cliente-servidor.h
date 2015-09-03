/*
 * cliente-servidor.h
 *
 *  Created on: 1/6/2015
 *      Author: utnso
 */

#include <sys/select.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <commons/string.h>

#ifndef CLIENTE_SERVIDOR_H_
#define CLIENTE_SERVIDOR_H_

void validarError(int validar, char* mensaje);

int crearSocket();

void asociarAPuerto(int socket, int puerto);

void liberarSocket(int socket);

void obviarErrorDeIpEnUso(int socket);

void conectarAServidor(int socket,char* direccionIp,int puerto);

int enviarMensaje(int socketCliente, void* buffer,int tamanioBuffer);

int recibirMensaje(int socketCliente, void* buffer,int tamanioBuffer);

int recibirMensajeCompleto(int socketCliente, void* buffer,int tamanioBuffer);

void escucharConexiones(int socket, int maximasConexionesEntrantes);

int crearSocketParaAceptarSolicitudes(int socketServidor);

void* recibirBloque(size_t* tamanioBloque,int socketEnviador);

void serializarIntYCadena(int entero,char* cadena,void* bloque);

void deserializarIntYCadena(int* entero,char* cadena,int tamanioBloque,void* bloque);

#endif /* CLIENTE_SERVIDOR_H_ */
