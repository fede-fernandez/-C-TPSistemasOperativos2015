/*
 * select.h
 *
 *  Created on: 9/9/2015
 *      Author: utnso
 */

#ifndef COMMONSDEASEDIO_SELECT_H_
#define COMMONSDEASEDIO_SELECT_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

t_list* crearListaDesdeEstructura(fd_set estructura,int maximoSocket);

fd_set crearEstructuraDesdeLista(t_list* lista);

//Solo pido que el socket del servidor lo pongan primero en la lista
bool filtrarListas(t_list* listaPrincipal,t_list* listaFiltrada);

void cargarEnLista(t_list* lista,int socket);

int mayorDeLista(t_list* lista);

int maximoEntre(int x, int y);

void limpiarLista(t_list* lista);

void quitarElementoDeLista(t_list* lista,int elemento);

//void agregarElementoALista(t_list* lista,int elemento);

bool perteneceALista(t_list* lista,int elemento);

void mostrarLista(t_list* lista);

#endif /* COMMONSDEASEDIO_SELECT_H_ */
