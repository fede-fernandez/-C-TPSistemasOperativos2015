/*
 * validaciones.h
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#ifndef VALIDACIONES_H_
#define VALIDACIONES_H_

#include "configuracion.h"
#include "funcionesMemoria.h"

//tipoEstructuraMemoria* datosMemoria;

bool estaHabilitadaLaTLB();

bool procesoExiste(int pid);

bool excedeMaximoDeMarcos(int pid);

bool TLBLlena();

bool RAMLlena();

bool tamanioDePaginaMayorAlSoportado(char* texto);

bool numeroDePaginaIncorrecto(int nroPagina,int pid);

#endif /* VALIDACIONES_H_ */
