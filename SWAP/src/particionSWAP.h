/*
 * particionSWAP.h
 *
 *  Created on: 14/9/2015
 *      Author: utnso
 */

#ifndef PARTICIONSWAP_H_
#define PARTICIONSWAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>

#define CARACTER_VACIO '\0'


//////////////FUNCIONES PARA EL ARCHIVO DE PARTICION///////////////////////

void inicializarParticion(char* nombreDeParticion,int tamanioDePagina,int cantidadDePaginas);

void escribirBloqueMapeado(char* archivo,char* contenidoAEscribir,int numDeBloque, int tamanioDeBloque);

char* leerBloqueMapeado(char* archivo,int numDeBloque, int tamanioDeBloque);



#endif /* PARTICIONSWAP_H_ */
