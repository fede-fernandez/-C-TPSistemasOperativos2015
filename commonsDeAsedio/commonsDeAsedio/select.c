/*
 * select.c
 *
 *  Created on: 9/9/2015
 *      Author: utnso
 */

#include "select.h"

t_list* crearListaDesdeEstructura(fd_set estructura,int socketMayor){

	t_list* lista = list_create();

	int var;

	for (var = socketMayor; var >0 ; var--) {

		if(FD_ISSET(var,&estructura)!=0)
			cargarEnLista(lista,var);
	}

	return lista;
}

fd_set crearEstructuraDesdeLista(t_list* lista){

	fd_set estructura;

	FD_ZERO(&estructura);

	int var;

	int* actual;

	for (var = 0; var < list_size(lista); ++var) {


		actual = list_get(lista,var);


		FD_SET(*actual,&estructura);

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

bool filtrarListas(t_list* listaPrincipal,t_list* listaFiltrada){

	/*struct timeval tiempoEspera;

	tiempoEspera.tv_sec = 1;

	tiempoEspera.tv_usec = 250000;*/

	fd_set estructuraFiltro;

	limpiarLista(listaFiltrada);

	int resultado,maximoSocket = mayorDeLista(listaPrincipal);

	do{
		FD_ZERO(&estructuraFiltro);

		estructuraFiltro = crearEstructuraDesdeLista(listaPrincipal);

		resultado = select(maximoSocket+1,&estructuraFiltro,NULL,NULL,NULL);

	}while(resultado<=0);

	listaFiltrada = crearListaDesdeEstructura(estructuraFiltro,maximoSocket);

	int* socketServidor = list_get(listaPrincipal,0);

	return perteneceALista(listaFiltrada,*socketServidor);

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

	void* buffer = malloc(sizeof(*elemento));

	buffer = elemento;

	list_add(lista,buffer);
}

bool perteneceALista(t_list* lista,int elemento){

	fd_set estructura = crearEstructuraDesdeLista(lista);

	return (FD_ISSET(elemento,&estructura)!=0);
}

void mostrarLista(t_list* lista){

	int var;

	int* actual;

	for (var = 0; var < list_size(lista); ++var) {

		actual = list_get(lista,var);

		printf("%d\n",*actual);
	}

}
