/*
 * log.c
 *
 *  Created on: 27/10/2015
 *      Author: ronan
 */

#include "log.h"


t_log* crearLoggerParaSeguimiento(char* rutaDeArchivoDeLog, char* nombreDeProcesoALogear){

	return log_create(rutaDeArchivoDeLog,nombreDeProcesoALogear,false,LOG_LEVEL_TRACE);
}




t_log* crearLoggerParaErrores(char* rutaDeArchivoDeLog, char* nombreDeProcesoALogear){

	return log_create(rutaDeArchivoDeLog,nombreDeProcesoALogear,false,LOG_LEVEL_ERROR);
}



void destruirLogger(t_log* logger){

	log_destroy(logger);
}



void logearSeguimiento(char* mensajeALogear,  t_log* logger){

	log_trace(logger,mensajeALogear);
}



void logearError(char* mensajeALogear, t_log* logger){

	log_error(logger,mensajeALogear);
}
