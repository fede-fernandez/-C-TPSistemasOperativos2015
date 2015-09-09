/*
 * sockets.c
 *
 *  Created on: 8/9/2015
 *      Author: utnso
 */
#include "sockets.h"

void administrarSockets(int puerto){

	fd_set socketsLectura,socketsEscritura,socketsExcepciones;

	FD_ZERO(&socketsEscritura);

	FD_ZERO(&socketsExcepciones);

	FD_ZERO(&socketsLectura);

	bool administraSockets = true;

	int socketAdministrador = crearSocket(),ultimoSocketLlegado, maximoSocket = 0,resultado;

	asociarAPuerto(socketAdministrador,puerto);

	FD_SET(socketAdministrador,&socketsEscritura);

	FD_SET(socketAdministrador,&socketsExcepciones);

	FD_SET(socketAdministrador,&socketsLectura);

	ultimoSocketLlegado = socketAdministrador;

	while(administraSockets){

		maximoSocket = maximoEntre(ultimoSocketLlegado,maximoSocket);

		resultado = select(maximoSocket,&socketsLectura,&socketsEscritura,&socketsExcepciones,NULL);


	}






}

int maximoEntre(int x, int y){

	if(x>y)
		return x;

	return y;
}


