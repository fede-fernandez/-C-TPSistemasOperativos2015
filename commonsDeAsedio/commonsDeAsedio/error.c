/*
 * error.c
 *
 *  Created on: 5/9/2015
 *      Author: utnso
 */

#include "error.h"

void validarErrorYAbortar(int condicionAValidar,char* mensajeEnCasoDeError){

	if(!condicionAValidar){
		perror(mensajeEnCasoDeError);
		abort();
	}
}
