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

int cualReemplazarTLB();

int cualReemplazarRAM();

int cualReemplazarRAMFIFO(t_list* listaAccesos);

int cualReemplazarRAMLRU(t_list* listaAccesos);

int cualReemplazarRAMCLOCKM();

int cualReemplazarTLBFIFO();

int cualReemplazarTLBLRU();

int cualReemplazarTLBCLOCKM();

void agregarAccesoPorAlgoritmo(int nroPagina,int pid);//Agrega un acceso dependiendo del algoritmo a utilizar

void inicializarPorLRU(t_list* listaParaAlgoritmo,int paginasPedidas);//Inicializa la lista para ram de accesos por lru

void inicializarPorAlgoritmo(tipoTablaPaginas* tablaDePagina);//Inicializa la lista de ram dependiendo el algoritmo

int ejecutarAlgoritmo(int* nroPagina,int pid,bool* estaModificada);//Setea el nroPagina elegido y si esta modificada, borra el acceso de la lista y retorna la posicion en ram

void agregarAccesoPorFIFO(tipoTablaPaginas* tablaDePaginas,int nroPagina);

void agregarAccesoPorLRU(tipoTablaPaginas* tablaDePaginas,int nroPagina);

void agregarAccesoPorCLOCKM(tipoTablaPaginas* tablaDePaginas,int nroPagina);

#endif /* ALGORITMOS_H_ */
