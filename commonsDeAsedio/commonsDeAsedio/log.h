/*
 * log.h
 *
 *  Created on: 27/10/2015
 *      Author: ronan
 */

#ifndef COMMONSDEASEDIO_LOG_H_
#define COMMONSDEASEDIO_LOG_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>


void logearSeguimiento(char* mensajeALogear, char* rutaDeArchivoDeLog, char* nombreDeProceso);

void logearError(char* mensajeALogear, char* rutaDeArchivoDeLog, char* nombreDeProceso);


#endif /* COMMONSDEASEDIO_LOG_H_ */
