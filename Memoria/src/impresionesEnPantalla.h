/*
 * funcionesSeñales.h
 *
 *  Created on: 7/11/2015
 *      Author: utnso
 */

#ifndef IMPRESIONESENPANTALLA_H_
#define IMPRESIONESENPANTALLA_H_

#include "funcionesMemoria.h"

void funcionPrueba(tipoEstructuraMemoria* datosMemoria);

void mostarEstado(tipoEstructuraMemoria* datosMemoria);

void mostrarTLB(tipoEstructuraMemoria* datosMemoria);

void mostrarTipoTLB(int posicionDeTabla,tipoEstructuraMemoria* datosMemoria);

void mostrarTablasDePaginas(tipoEstructuraMemoria* datosMemoria);

void mostrarTablaDePaginas(int posicionDeTabla,tipoEstructuraMemoria* datosMemoria);

void mostrarTasaTLBPeriodicamente();

void imprimirPaginas(t_list* frames);

void imprimirTablas(t_list* tablas);

void mostrarTasaPageFaults(int posicion);

void mostrarTasasPageFaults(tipoEstructuraMemoria* datosMemoria);

void mostrarRAM(t_list* ram);

#endif /* IMPRESIONESENPANTALLA_H_ */

