/*
 * select.c
 *
 *  Created on: 9/9/2015
 *      Author: utnso
 */

#include "select.h"

t_list* crearListaDesdeEstructura(fd_set* estructura,int socketMayor){

	t_list* lista = list_create();

	int var;

	for (var = socketMayor; var >0 ; var--) {

		if(FD_ISSET(var,estructura)!=0)
			cargarEnLista(lista,var);
	}

	return lista;
}

fd_set* crearEstructuraDesdeLista(t_list* lista){

	fd_set* estructura;

	FD_ZERO(estructura);

	int var;

	int* actual;

	for (var = 1; var < mayorDeLista(lista) ; ++var) {

		actual = list_get(lista,var);

		FD_SET(*actual,estructura);

	}
	return estructura;
}

void cargarEnLista(t_list* lista,int socket){

	int* buffer = malloc(sizeof(int));

	memcpy(buffer,&socket,sizeof(int));

	list_add(lista,buffer);
}


int maximoEntre(int x, int y){

	if(x>y)
		return x;

	return y;
}

int mayorDeLista(t_list* lista){

	int var,maximo = 0;

	int* actual;

	for (var = 0; var < list_size(lista); ++var) {

		actual = list_get(lista,var);

		maximo = maximoEntre(maximo,*actual);
	}

	return maximo;
}

void filtrarListas(t_list* listaPrincipal,t_list* listaLectura,t_list* listaEscritura,t_list* listaExcepciones){

	fd_set* estructuraLectura;

	fd_set* estructuraEscritura;

	fd_set* estructuraExcepciones;

	limpiarLista(listaEscritura);

	limpiarLista(listaLectura);

	limpiarLista(listaExcepciones);

	int resultado,maximoSocket = mayorDeLista(listaPrincipal);

	do{
		estructuraLectura = crearEstructuraDesdeLista(listaPrincipal);

		estructuraEscritura =  crearEstructuraDesdeLista(listaPrincipal);

		estructuraExcepciones = crearEstructuraDesdeLista(listaPrincipal);

		resultado = select(maximoSocket+1,estructuraLectura,estructuraEscritura,estructuraExcepciones,NULL);

	}while(resultado==-1);

	listaLectura = crearListaDesdeEstructura(estructuraLectura,maximoSocket);

	listaEscritura = crearListaDesdeEstructura(estructuraEscritura,maximoSocket);

	listaExcepciones = crearListaDesdeEstructura(estructuraExcepciones,maximoSocket);
}

void limpiarLista(t_list* lista){

int var;

for (var = 0; var < list_size(lista); ++var) {
	list_remove(lista,0);
}

}

void quitarElementoDeLista(t_list* lista,void* elemento){

	int var;

	for (var = 0; var < list_size(lista); ++var) {

		if(elemento==list_get(lista,var))
			break;
	}

	list_remove(lista,var);

}

void agregarElementoALista(t_list* lista,void* elemento){

	void* buffer = malloc(sizeof(elemento));

	buffer = elemento;

	list_add(lista,buffer);
}

bool perteneceALista(t_list* lista,int elemento){

	fd_set* estructura = crearEstructuraDesdeLista(lista);

	return (FD_ISSET(elemento,estructura)!=0);
}
