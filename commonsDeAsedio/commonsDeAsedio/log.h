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


t_log* crearLoggerParaSeguimiento(char* rutaDeArchivoDeLog, char* nombreDeProcesoALogear);

t_log* crearLoggerParaErrores(char* rutaDeArchivoDeLog, char* nombreDeProcesoALogear);

void destruirLogger(t_log* logger);

void logearSeguimiento(char* mensajeALogear,  t_log* logger);

void logearError(char* mensajeALogear, t_log* logger);


#endif /* COMMONSDEASEDIO_LOG_H_ */
