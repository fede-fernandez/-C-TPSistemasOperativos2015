/*
 * huecosSWAP.h
 *
 *  Created on: 13/9/2015
 *      Author: utnso
 */

#ifndef HUECOSSWAP_H_
#define HUECOSSWAP_H_

#include "estructurasSWAP.h"

#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/mmap.h>
#include <commons/string.h>

#define SI 1
#define NO 0

/////////////FUNCIONES PARA EL CONTROL DE HUECOS///////////////////////////////////

/********FUNCIONES QUE ATIENDEN LAS INSTRUCCIONES DEL LENGUAJE MANSISOP**************/
int reservarEspacio(t_list* listaDeHuecosUtilizados,int pidProceso,int cantDePaginasSolicitadas,tipoConfigSWAP* configuracion);
void liberarEspacio(t_list* listaDeHuecosUtilizados,int pidProceso);
char* leerPagina(t_list* listaDeHuecosUtilizados,int pidProceso,int dirLogicaDePagina,int tamanioDePagina,FILE* particion);
void escribirPagina(t_list* listaDeHuecosUtilizados,int pidProceso,char* contenidoAEscribir,int dirLogicaDePagina,int tamanioDePagina, FILE* particion);




tipoHuecoLibre* crearHuecoLibre(int inicio,int cantidadDePaginas);
void destruirHuecoLibre(tipoHuecoLibre* huecoLibre);
t_list* inicializarListaDeHuecosLibres(int cantidadDePaginas);

tipoHuecoUtilizado* crearHuecoUtilizado(int pidProceso,int inicio,int cantidadDePaginas);
void destruirHuecoUtilizado(tipoHuecoUtilizado* huecoUtilizado);
t_list* inicializarListaDeHuecosUtilizados();

int espacioDisponible(t_list* listaDeHuecosLibres,int tamanioDePagina);
int paginasDisponibles(t_list* listaDeHuecosUtilizados,int cantDePaginasTotal);


int tengoEspacioDisponible(t_list* listaDeHuecosUtilizados,int cantDePaginasTotal);//OK
int tengoEspacioContiguoDisponible(t_list* listaDeHuecosUtilizados,int cantDePaginasSolicitadas);//OK
void asignarEspacio(t_list* listaDeHuecosUtilizados,int pidProceso,int cantDePaginasSolicitadas, int indiceDeHuecoPrevio);
void compactar();



////////FUNCIONES AUXILIARES///////////
int espacioEntreDosHuecosUtilizados(tipoHuecoUtilizado* h1, tipoHuecoUtilizado* h2);
int paginaMaxima(tipoHuecoUtilizado* hueco);
tipoHuecoUtilizado* buscarHuecoDePID(t_list* listaDeHuecosUtilizados,int pidProcesoBuscado);
int traducirDireccionLogicaAFisica(tipoHuecoUtilizado* hueco,int dirLogicaDePagina);
void imprimirHueco(tipoHuecoUtilizado* hueco);
void imprimirListaDeHuecos(t_list* lista);

#endif /* HUECOSSWAP_H_ */
