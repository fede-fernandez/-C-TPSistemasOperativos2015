/*
 * sockets.h
 *
 *  Created on: 8/9/2015
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <stdbool.h>

void administrarSockets(int puerto);

int maximoEntre(int x, int y);

#endif /* SOCKETS_H_ */
