/*
 * particionSWAP.c
 *
 *  Created on: 14/9/2015
 *      Author: utnso
 */


#include "particionSWAP.h"

//////////////FUNCIONES PARA EL ARCHIVO DE PARTICION///////////////////////


FILE* inicializarParticion(char* nombreDeParticion,int tamanioDePagina,int cantidadDePaginas){

	char* instruccion=string_new();
	FILE* particion;

	printf("Inicializando particion...\n\n");


	sprintf(instruccion,"dd if=/dev/zero of=%s bs=%d count=%d",nombreDeParticion,tamanioDePagina,cantidadDePaginas);
	//sprintf(instruccion,"truncate -s %d %s",tamanioDeParticion,nombreDeParticion);
	system(instruccion);

	printf("Particion inicializada. \n");

	particion = fopen(nombreDeParticion,"r+");//modo actualizacion, el archivo debe existir

	return particion;
}
