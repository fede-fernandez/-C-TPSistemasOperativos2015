/*
 * configuracion.h
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/error.h>


#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define IP_SWAP "IP_SWAP"
#define PUERTO_SWAP "PUERTO_SWAP"
#define MAXIMO_MARCOS_POR_PROCESO "MAXIMO_MARCOS_POR_PROCESO"
#define CANTIDAD_MARCOS "CANTIDAD_MARCOS"
#define TAMANIO_MARCO "TAMANIO_MARCO"
#define ENTRADAS_TLB "ENTRADAS_TLB"
#define TLB_HABILITADA "TLB_HABILITADA"
#define RETARDO_MEMORIA "RETARDO_MEMORIA"
#define ALGORITMO_RAM "ALGORITMO_RAM"

/*******************ESTRUCTURAS*************************/

typedef struct{
	int puertoDeEscucha;
	char* ipSWAP;
	int puertoSWAP;
	int maximoDeMarcosPorProceso;
	int cantidadDeMarcos;
	int tamanioDeMarco;
	int entradasDeTLB;
	char* TLBHabilitada;
	int retardoDeMemoria;
	char* algoritmoRAM;
}tipoConfigMemoria;


typedef struct{
	t_list* listaRAM;
	t_list* listaTLB;
	t_list* listaTablaPaginas;
	int socketSWAP;
	int maximoSocket;
	tipoConfigMemoria* configuracion;
	bool* memoriaActiva;
	int socketCpus;
	//t_list* colaFIFOTLB;
	t_list* listaHuecosRAM;
	char tipoDeAlgoritmoRAM;
}tipoEstructuraMemoria;

typedef struct{
	int pid;
	int nroPagina;
}tipoAccesoAPaginaTLB;


typedef struct{
	int pid;
	int numeroDePagina;
	int posicionEnRAM;
}tipoTLB;

typedef struct{
	int pid;
	int paginasPedidas;
	int paginasAsignadas;
	int punteroParaAlgoritmo;
	t_list* frames;
	t_list* listaParaAlgoritmo;
}tipoTablaPaginas;

#define EN_SWAP -18
#define EN_RAM -17
#define NO_EXISTE -16

typedef struct{
	bool modificado;
	int posicionEnRAM;
	bool usado;
	int presente;
}tipoPagina;

tipoConfigMemoria* crearConfigMemoria();

void destruirConfigMemoria(tipoConfigMemoria* estructuraDeConfiguracion);

tipoConfigMemoria* cargarArchivoDeConfiguracionDeMemoria(char* rutaDelArchivoDeConfiguracion);


#endif /* CONFIGURACION_H_ */
