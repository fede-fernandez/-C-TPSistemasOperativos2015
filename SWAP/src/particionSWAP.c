/*
 * particionSWAP.c
 *
 *  Created on: 14/9/2015
 *      Author: utnso
 */


#include "particionSWAP.h"
#define SCRIPT "crearParticion.sh"

//////////////FUNCIONES PARA EL ARCHIVO DE PARTICION///////////////////////


FILE* inicializarParticion(char* nombreDeParticion,int tamanioDePagina,int cantidadDePaginas){

	char* instruccion = string_new();
	FILE* particion;

	printf("Inicializando particion...\n\n");

	sprintf(instruccion,"chmod +x %s",SCRIPT);
	system(instruccion);
	//sprintf(instruccion,"dd if=/dev/zero of=%s bs=%d count=%d",nombreDeParticion,tamanioDePagina,cantidadDePaginas);
	//sprintf(instruccion,"truncate -s %d %s",tamanioDePagina*cantidadDePaginas,nombreDeParticion);

	sprintf(instruccion,"./%s %s %d %d",SCRIPT,nombreDeParticion,tamanioDePagina,cantidadDePaginas);
	system(instruccion);

	printf("Particion inicializada. \n");

	particion = fopen(nombreDeParticion,"r+");//modo actualizacion, el archivo debe existir

	return particion;
}
