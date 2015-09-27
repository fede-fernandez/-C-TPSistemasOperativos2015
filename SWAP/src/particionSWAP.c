/*
 * particionSWAP.c
 *
 *  Created on: 14/9/2015
 *      Author: utnso
 */


#include "particionSWAP.h"


char* recuperarBloque(char* bloqueAVaciar);
char* completarBloque(char* bloqueACompletar, int tamanioDeBloque);


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




void escribirBloqueMapeado(char* rutaDeParticion,char* contenidoAEscribir,int numDeBloque, int tamanioDeBloque){

	FILE* archivo = fopen(rutaDeParticion,"r+");

	fseek(archivo,numDeBloque*tamanioDeBloque,SEEK_SET);

	char* aux = completarBloque(contenidoAEscribir,tamanioDeBloque);

	fwrite(aux,tamanioDeBloque,1,archivo);
}

char* leerBloqueMapeado(char* rutaDeParticion,int numDeBloque, int tamanioDeBloque){
	FILE* archivo = fopen(rutaDeParticion,"r+");

	char* leido = string_new();

	fseek(archivo,numDeBloque*tamanioDeBloque,SEEK_SET);

	fread(leido,tamanioDeBloque,1,archivo);

	char* aux = recuperarBloque(leido);

	return aux;
}


/////////AUXILIARES/////////


char* completarBloque(char* bloqueACompletar, int tamanioDeBloque){

	int cantACompletar = tamanioDeBloque - string_length(bloqueACompletar);
	char* aux = string_duplicate(bloqueACompletar);

	string_append(&aux,string_repeat(CARACTER_VACIO,cantACompletar));

	return aux;
}

char* recuperarBloque(char* bloqueAVaciar){
	char* bloque = string_new();
	int i;

	for (i = 0; i < string_length(bloqueAVaciar); ++i) {
		if (bloqueAVaciar[i] == CARACTER_VACIO)
			break;
	}

	bloque = string_substring_until(bloqueAVaciar,i);

	return bloque;
}
