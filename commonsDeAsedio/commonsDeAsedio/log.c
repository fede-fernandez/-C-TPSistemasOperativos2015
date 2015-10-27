/*
 * log.c
 *
 *  Created on: 27/10/2015
 *      Author: ronan
 */

#include "log.h"

void logearSeguimiento(char* mensajeALogear, char* rutaDeArchivoDeLog, char* nombreDeProceso){
	t_log* log = log_create(rutaDeArchivoDeLog,nombreDeProceso,false,LOG_LEVEL_TRACE);

	log_trace(log,mensajeALogear);

	log_destroy(log);
}


void logearError(char* mensajeALogear, char* rutaDeArchivoDeLog, char* nombreDeProceso){

	t_log* log = log_create(rutaDeArchivoDeLog,nombreDeProceso,false,LOG_LEVEL_ERROR);

	log_error(log,mensajeALogear);

	log_destroy(log);
}
