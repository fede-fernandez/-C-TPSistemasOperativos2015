/*
 * huecosSWAP.c
 *
 *  Created on: 13/9/2015
 *      Author: utnso
 */


#include "estructurasSWAP.h"
#include "huecosSWAP.h"
#include <commons/collections/list.h>

/////////////////////LISTAS DE HUECOS///////////////////////



tipoHuecoLibre* crearHuecoLibre(int inicio,int cantidadDePaginas){

	tipoHuecoLibre* aux = malloc(sizeof(tipoHuecoLibre));
	aux->baseDeMProc = inicio;
	aux->cantidadDePaginasQueOcupa = cantidadDePaginas;

	return aux;
}

void destruirHuecoLibre(tipoHuecoLibre* huecoLibre){
	free(huecoLibre);
}

t_list* inicializarListaDeHuecosLibres(int cantidadDePaginas){

	t_list*	listaDeHuecosLibres = list_create();
	list_add(listaDeHuecosLibres,crearHuecoLibre(0,cantidadDePaginas));
	return listaDeHuecosLibres;
}

tipoHuecoUtilizado* crearHuecoUtilizado(int pidProceso,int inicio,int cantidadDePaginas){

	tipoHuecoUtilizado* aux = malloc(sizeof(tipoHuecoUtilizado));
	aux->pid = pidProceso;
	aux->baseDeMProc = inicio;
	aux->cantidadDePaginasQueOcupa = cantidadDePaginas;

	return aux;
}

void destruirHuecoUtilizado(tipoHuecoUtilizado* huecoUtilizado){
	free(huecoUtilizado);
}

t_list* inicializarListaDeHuecosUtilizados(){

	t_list*	listaDeHuecosUtilizados = list_create();
	return listaDeHuecosUtilizados;
}


int paginasDisponibles(t_list* listaDeHuecosUtilizados,int cantDePaginasTotal){
	int pagDisponibles = 0;
	int i;
	tipoHuecoUtilizado* aux1 = malloc(sizeof(tipoHuecoUtilizado));
	tipoHuecoUtilizado* aux2 = malloc(sizeof(tipoHuecoUtilizado));

	for (i = 0; i < list_size(listaDeHuecosUtilizados); ++i) {
		if (!cantDePaginasTotal == i) {

			aux1 = list_get(listaDeHuecosUtilizados,i);
			aux2 = list_get(listaDeHuecosUtilizados,i+1);

			pagDisponibles += espacioEntreDosHuecosUtilizados(aux1,aux2);
		}
	}

	destruirHuecoUtilizado(aux1);
	destruirHuecoUtilizado(aux2);

	return pagDisponibles;
}//OK

int espacioDisponible(t_list* listaDeHuecosLibres,int tamanioDePagina){
	int espacioDisponible = 0;
	int i;
	tipoHuecoLibre* aux;

	for (i = 0; i < list_size(listaDeHuecosLibres); i++) {
		aux = list_get(listaDeHuecosLibres,i);
		espacioDisponible+= (tamanioDePagina*(aux->cantidadDePaginasQueOcupa));
	}

	return espacioDisponible;
}//OK

int reservarEspacio(t_list* listaDeHuecosLibres,t_list* listaDeHuecosUtilizados,int pidProceso,int cantDePaginasSolicitadas,tipoConfigSWAP* configuracion){
	//if(tengoEspacioDisponible)
	//	if(!tengoEspacioContiguo)
	//		compactar
	//	asignarEspacio
	//else
	//	error

	return 0;
}

/////////////////FUNCIONES PRIVADAS//////////////


int tengoEspacioDisponible(t_list* listaDeHuecosUtilizados,int cantDePaginasTotal){
	if (paginasDisponibles(listaDeHuecosUtilizados,cantDePaginasTotal) > 0) {
		return SI;
	} else {
		return NO;
	}
}//OK

int tengoEspacioContiguoDisponible(t_list* listaDeHuecosUtilizados,int cantDePaginasSolicitadas){

	int resultado = 0;
	tipoHuecoUtilizado* aux1;
	tipoHuecoUtilizado* aux2;

	int i=0;
	while(i<list_size(listaDeHuecosUtilizados) && resultado==0){

		aux1 = list_get(listaDeHuecosUtilizados,i);
		aux2 = list_get(listaDeHuecosUtilizados,i+1);

		if (cantDePaginasSolicitadas <= espacioEntreDosHuecosUtilizados(aux1,aux2)) {
			resultado = i;
		}

		i++;
	}

	return resultado;
}

void compactar(){
}

void asignarEspacio(t_list* listaDeHuecosUtilizados,int pidProceso,int cantDePaginasSolicitadas,int indiceHuecoPrevio){
	//crearEstructuraOcupado
	tipoHuecoUtilizado* aux = list_get(listaDeHuecosUtilizados,indiceHuecoPrevio);
	tipoHuecoUtilizado* huecoNuevo = crearHuecoUtilizado(pidProceso,paginaMaxima(aux),cantDePaginasSolicitadas);

	//agregarAListaOcupados
	list_add_in_index(listaDeHuecosUtilizados,indiceHuecoPrevio+1,huecoNuevo);

	//actualizarParticionPorAsignacion
}


///////FUNCIONES AUXILIARES/////
int espacioEntreDosHuecosUtilizados(tipoHuecoUtilizado* h1, tipoHuecoUtilizado* h2){
	return (h2->baseDeMProc - (h1->baseDeMProc + h1->cantidadDePaginasQueOcupa));
}

int paginaMaxima(tipoHuecoUtilizado* hueco){
	return (hueco->baseDeMProc + hueco->cantidadDePaginasQueOcupa);
}


