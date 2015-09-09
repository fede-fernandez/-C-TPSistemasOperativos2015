/*
 * select.h
 *
 *  Created on: 9/9/2015
 *      Author: utnso
 */

#ifndef COMMONSDEASEDIO_SELECT_H_
#define COMMONSDEASEDIO_SELECT_H_

#include <commons/collections/list.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

t_list* crearListaDesdeEstructura(fd_set* estructura,int maximoSocket);

fd_set* crearEstructuraDesdeLista(t_list* lista);

void filtrarListas(t_list* listaPrincipal,t_list* listaLectura,t_list* listaEscritura,t_list* listaExcepciones);

void cargarEnLista(t_list* lista,int socket);

int mayorDeLista(t_list* lista);

int maximoEntre(int x, int y);

#endif /* COMMONSDEASEDIO_SELECT_H_ */
