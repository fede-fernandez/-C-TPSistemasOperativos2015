/*
 * funcionesMemoria.h
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

#ifndef FUNCIONESMEMORIA_H_
#define FUNCIONESMEMORIA_H_

#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>


#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define IP_SWAP "IP_SWAP"
#define PUERTO_SWAP "PUERTO_SWAP"
#define MAXIMO_MARCOS_POR_PROCESO "MAXIMO_MARCOS_POR_PROCESO"
#define CANTIDAD_MARCOS "CANTIDAD_MARCOS"
#define TAMANIO_MARCO "TAMANIO_MARCO"
#define ENTRADAS_TLB "ENTRADAS_TLB"
#define TLB_HABILITADA "TLB_HABILITADA"
#define RETARDO_MEMORIA "RETARDO_MEMORIA"

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
}tipoConfigMemoria;


tipoConfigMemoria* crearConfigMemoria();

void destruirConfigMemoria(tipoConfigMemoria* estructuraDeConfiguracion);

tipoConfigMemoria* cargarArchivoDeConfiguracionDeMemoria(char* rutaDelArchivoDeConfiguracion);

void tratarEscritura(int socketParaCpus,int socketParaSwap,int socketParaEscribir);

void tratarLectura(int socketParaCpus,int socketParaSwap,int socketParaLeer);

void tratarLecturas(int socketParaCpus,int socketParaSwap,t_list* listaLectura);

void tratarEscrituras(int socketParaCpus,int socketParaSwap,t_list* listaEscritura);


/***************instrucciones*******************/
void ejecutarInstrunccion (tipoInstruccion instruccionCPU);

/****iniciar N*****/
bool reservarMemoriaEnSwap(int cantidadDePaginas, int procesoID);

bool reservarMemoriaEnRam(int cantidadDePaginas, int procesoID);

void cancelarInicializacion(int procesoID);

void reservarMemoriaParaProceso(int cantidadDePaginas, int procesoID);

/*******leer pagina***********/
bool buscarPaginaEnRam(int pagina, int procesoID, char* contenidoDePagina);

bool buscarPaginaEnSwap(int pagina, int procesoID);

void enviarPaginaACPU(int procesoID, char* contenidoDePagina);

void pedirPaginaDesdeSwapARam(int proceso, int numeroDePagina, char* contenidoDePagina);

void leerPagina(int numeroPagina, int procesoID);

#endif /* FUNCIONESMEMORIA_H_ */
