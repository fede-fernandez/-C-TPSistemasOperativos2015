/*
 * algoritmos.h
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include "configuracion.h"
#include "funcionesMemoria.h"

#define FIFO 1
#define LRU 2
#define CLOCK_MODIFICADO 3

//tipoEstructuraMemoria* datosMemoria;

void setearParaAlgoritmos();//Setea el tipo de algoritmo a utilizar

int ejecutarAlgoritmoDeReemplazo(int* nroPagina,int pid,tipoTablaPaginas* tablaDePagina);

tipoAccesoAPaginaTLB* cualReemplazarTLB();

int cualReemplazarRAM();

int cualReemplazarRAMFIFO(t_list* listaAccesos);

int cualReemplazarRAMLRU(t_list* listaAccesos);

int cualReemplazarRAMCLOCKM(tipoTablaPaginas* tablaDePagina);

bool ejecutarPaso1CLOCKM(tipoTablaPaginas* tablaDePagina,int* nroPaginaAReemplazar);

bool ejecutarPaso2CLOCKM(tipoTablaPaginas* tablaDePagina,int* nroPaginaAReemplazar);

tipoAccesoAPaginaTLB* cualReemplazarTLBFIFO();

void agregarAccesoPorAlgoritmo(int nroPagina,int pid);//Agrega un acceso dependiendo del algoritmo a utilizar

void inicializarPorLRU(t_list* listaParaAlgoritmo,int paginasPedidas);//Inicializa la lista para ram de accesos por lru

void inicializarPorAlgoritmo(tipoTablaPaginas* tablaDePagina);//Inicializa la lista de ram dependiendo el algoritmo

int ejecutarAlgoritmo(int* nroPagina,int pid,bool* estaModificada);//Setea el nroPagina elegido y si esta modificada, borra el acceso de la lista y retorna la posicion en ram

void agregarAccesoPorFIFO(int nroPagina,int pid);//Solo para TLB

void agregarAccesoPorLRU(tipoTablaPaginas* tablaDePaginas,int nroPagina);

void agregarAccesoPorCLOCKM(tipoTablaPaginas* tablaDePaginas,int nroPagina);

void quitarAccesoTLB(int nroPaginaAReemplazar,int pid);

int dondeEstaAccesoTLB(int nroPagina,int pid);

#endif /* ALGORITMOS_H_ */
