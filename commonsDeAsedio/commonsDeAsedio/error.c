/*
 * error.c
 *
 *  Created on: 5/9/2015
 *      Author: utnso
 */

#include "error.h"

void validar(int condicionAValidar,char* mensajeEnCasoDeError){

	if(!condicionAValidar){
		perror(mensajeEnCasoDeError);
		exit(-1);
	}
}
