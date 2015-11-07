/*
 * funcionesSeñales.h
 *
 *  Created on: 7/11/2015
 *      Author: utnso
 */

#ifndef FUNCIONESSEÑALES_H_
#define FUNCIONESSEÑALES_H_

#include "funcionesMemoria.h"

void funcionPrueba(tipoEstructuraMemoria* datosMemoria);

void mostarEstado(tipoEstructuraMemoria* datosMemoria);

void mostrarTLB(tipoEstructuraMemoria* datosMemoria);

void mostrarTipoTLB(int posicionDeTabla,tipoEstructuraMemoria* datosMemoria);

void mostrarTablasDePaginas(tipoEstructuraMemoria* datosMemoria);

void mostrarTablaDePaginas(int posicionDeTabla,tipoEstructuraMemoria* datosMemoria);

#endif /* FUNCIONESSEÑALES_H_ */

