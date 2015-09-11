/*
 * funcionesSWAP.h
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

#ifndef FUNCIONESSWAP_H_
#define FUNCIONESSWAP_H_

#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/error.h>
#include <string.h>
#include <commonsDeAsedio/mmap.h>
#include <commons/collections/list.h>

#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define NOMBRE_SWAP "NOMBRE_SWAP"
#define CANTIDAD_PAGINAS "CANTIDAD_PAGINAS"
#define TAMANIO_PAGINA "TAMANIO_PAGINA"
#define RETARDO_COMPACTACION "RETARDO_COMPACTACION"

typedef struct{
	int puertoDeEscucha;
	char* nombreDeSWAP;
	int cantidadDePaginas;
	int tamanioDePagina;
	int retardoDeCompactacion;
}tipoConfigSWAP;

typedef struct{
	int pid;
	int baseDeMProc;
	int cantidadDePaginasQueOcupa;
}tipoHuecoUtilizado;

typedef struct{
	int baseDeMProc;
	int cantidadDePaginasQueOcupa;
}tipoHuecoLibre;

////////////////////FUNCIONES PARA EL MANEJO DE ARCHIVO DE CONFIGURACION///////////////////////

tipoConfigSWAP* crearConfigSWAP();

void destruirConfigSWAP (tipoConfigSWAP* estructuraDeConfiguracion);

tipoConfigSWAP* cargarArchivoDeConfiguracionDeSWAP(char* rutaDelArchivoDeConfiguracion);



//////////////FUNCIONES PARA EL ARCHIVO DE PARTICION///////////////////////

//FILE* inicializarParticion(char* nombreDeParticion,int tamanioDePagina,int cantidadDePaginas);




////////////////////////////////////////////////





tipoHuecoLibre* crearHuecoLibre(int inicio,int cantidadDePaginas);
void destruirHuecoLibre(tipoHuecoLibre* huecoLibre);
t_list* inicializarListaDeHuecosLibres(int cantidadDePaginas);
int espacioDisponible(t_list* listaDeHuecosLibres,int tamanioDePagina);


#endif /* FUNCIONESSWAP_H_ */
