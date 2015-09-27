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
#include <stdbool.h>

#include <commonsDeAsedio/mmap.h>
#include <commonsDeAsedio/estructuras.h>

#include <commons/string.h>

bool baseMenor(tipoHuecoUtilizado* h1, tipoHuecoUtilizado* h2);


/********FUNCIONES QUE ATIENDEN LAS INSTRUCCIONES DEL LENGUAJE MANSISOP**************/
int reservarEspacio(t_list* listaDeHuecosUtilizados,int pidProcesoNuevo, int cantPaginasSolicitadas,int cantDePaginasDeSWAP,int tamanioDePagina, char* particion);
void liberarEspacio(t_list* listaDeHuecosUtilizados,int pidProceso);
char* leerPagina(t_list* listaDeHuecosUtilizados,int pidProceso,int dirLogicaDePagina,int tamanioDePagina,char* particion);
void escribirPagina(t_list* listaDeHuecosUtilizados,int pidProceso,char* contenidoAEscribir,int dirLogicaDePagina,int tamanioDePagina, char* particion);




tipoHuecoUtilizado* crearHuecoUtilizado(int pidProceso,int inicio,int cantidadDePaginas);
void destruirHuecoUtilizado(tipoHuecoUtilizado* huecoUtilizado);
t_list* inicializarListaDeHuecosUtilizados();


int cantidadDePaginasDisponibles(t_list* listaDeHuecosUtilizados,int cantTotalDePaginas);
int baseParaMProcSiTengoEspacioContiguo(t_list* listaDeHuecosUtilizados, int cantDePaginasSolicitadas, int cantTotalDePaginas);


void asignarEspacio(t_list* listaDeHuecosUtilizados,int pidProceso,int cantDePaginasSolicitadas, int base);
void compactar();


void compactacionAlpha(t_list* listaDeHuecosUtilizados, char* particion,int tamanioDePagina);

void moverHueco(tipoHuecoUtilizado* hueco,char* particion, int ultimaPaginaEscrita,int tamanioDePagina);
void moverPagina(char* particion, int dirFisVieja, int dirFisNueva,int tamanioDePagina);

int traducirDireccionLogicaAFisica(tipoHuecoUtilizado* hueco,int dirLogicaDePagina);
int espacioEntreDosHuecosUtilizados(tipoHuecoUtilizado* h1, tipoHuecoUtilizado* h2);



////////FUNCIONES AUXILIARES///////////
void imprimirHueco(tipoHuecoUtilizado* hueco);
void imprimirListaDeHuecos(t_list* lista);

#endif /* HUECOSSWAP_H_ */
