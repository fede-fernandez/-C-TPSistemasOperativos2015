/*
 * huecosSWAP.h
 *
 *  Created on: 13/9/2015
 *      Author: utnso
 */

#ifndef HUECOSSWAP_H_
#define HUECOSSWAP_H_

#include "estructurasSWAP.h"
#include "particionSWAP.h"

#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <commonsDeAsedio/mmap.h>
#include <commonsDeAsedio/estructuras.h>
#include <commonsDeAsedio/log.h>

#include <commons/string.h>



#define ERROR_NO_HAY_ESPACIO_EN_SWAP "No se pudo asignar la pagina. No hay espacio disponible en SWAP"
#define OK_ESPACIO_RESERVADO "Espacio en SWAP reservado"
#define OK_ESPACIO_LIBERADO "Espacio en SWAP liberado"
#define OK_PAGINA_ESCRITA "Pagina escrita correctamente en SWAP"




//-----------Huecos utilizados------------
tipoHuecoUtilizado* crearHuecoUtilizado(int pidProceso,int inicio,int cantidadDePaginas);
void destruirHuecoUtilizado(tipoHuecoUtilizado* huecoUtilizado);
t_list* inicializarListaDeHuecosUtilizados();
int ultimaPaginaEscrita(t_list* listaDeHuecosUtilizados);
tipoHuecoUtilizado* buscarHuecoUtilizadoPorPID(t_list* listaDeHuecosUtilizados,int pidProcesoBuscado);

//------------Huecos libres------------
tipoHuecoLibre* crearHuecoLibre(int base, int cantidadDePaginas);
void destruirHuecoLibre(tipoHuecoLibre* huecoLibre);
t_list* inicializarListaDeHuecosLibres(int cantidadDePaginas);
int cantidadDePaginasDisponibles(t_list* listaDeHuecosLibres);
bool tengoEspacioContiguo(t_list* listaDeHuecosLibres, int cantidadDePaginasSolicitadas, int* baseParaNuevoProceso);
void ocuparEspacioLibre(t_list* listaDeHuecosLibres, int baseOcupada, int cantidadDePaginasSolicitadas);
bool sonContiguosAIzquierda(tipoHuecoLibre* unHueco, tipoHuecoLibre* otroHueco);
bool sonContiguosADerecha(tipoHuecoLibre* unHueco, tipoHuecoLibre* otroHueco);
int ultimaPaginaDeHuecoLibre(tipoHuecoLibre* unHueco);
void unirHuecosLibresAIzquierda(tipoHuecoLibre* unHueco, tipoHuecoLibre* otroHueco);
void unirHuecosLibresADerecha(tipoHuecoLibre* unHueco, tipoHuecoLibre* otroHueco);
bool estaVacio(tipoHuecoLibre* unHueco);
void actualizarListaDeLibres(t_list* listaDeHuecosLibres);




//-------------Funciones auxiliares para reservar espacio----------//
void asignarEspacio(t_list* listaDeHuecosUtilizados, t_list* listaDeHuecosLibres, int pidProceso,int base,int cantDePaginasSolicitadas);
void compactacionAlpha(t_list* listaDeHuecosUtilizados,t_list* listaDeHuecosLibres, char* particion,int tamanioDePagina, int cantidadDePaginasSWAP, int retardoDeCompactacion,t_log* logger);
void moverHueco(tipoHuecoUtilizado* hueco,char* particion, int ultimaPaginaEscrita,int tamanioDePagina);
void moverPagina(char* particion, int dirFisVieja, int dirFisNueva,int tamanioDePagina);


//-----------Funciones auxiliares para liberar espacio----------//
void liberarProcesoDeListaDeHuecosUtilizadosYDeParticion(t_list* listaDeHuecosUtilizados,char* rutaDeParticion, int tamanioDePagina, int pidALiberar, int* base, int* cantidadDePaginas);
void actualizarListaDeHuecosLibres(t_list* listaDeHuecosLibres, int base, int cantidadDePaginas);



////////FUNCIONES AUXILIARES///////////

void imprimirListaDeHuecosUtilizados(t_list* lista);
void imprimirListaDeHuecosLibres(t_list* lista);
int traducirDireccionLogicaAFisica(tipoHuecoUtilizado* hueco,int dirLogicaDePagina);
bool baseMenor(tipoHuecoUtilizado* h1, tipoHuecoUtilizado* h2);
bool baseMenorDeHuecoLibre(tipoHuecoLibre* unHueco, tipoHuecoLibre* otroHueco);

#endif /* HUECOSSWAP_H_ */
