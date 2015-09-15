/*
 ============================================================================
 Name        : mmap.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "mmap.h"
#include <sys/stat.h>
#include <fcntl.h>



int tamanioDeArchivo(int descriptorDeArchivo){
	struct stat buffer;
	fstat(descriptorDeArchivo, &buffer);
	return buffer.st_size;
}

void* mapearBloqueDeArchivoAMemoria(FILE* archivoAMapear,int numeroDeBloqueNodo){

	return mmap(NULL,VEINTE_MB,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(archivoAMapear),numeroDeBloqueNodo*VEINTE_MB);
}

void liberarMemoriaDeBloqueDeArchivoMapeado(void* resultadoDeMapeo){
	munmap(resultadoDeMapeo,VEINTE_MB);
}

void* mapearArchivoCompleto(FILE* archivoAMapear){
	void* mapeo;
	int tamanio, descriptorDeArchivoAMapear;

	descriptorDeArchivoAMapear = fileno(archivoAMapear);
	tamanio = tamanioDeArchivo(descriptorDeArchivoAMapear);

	mapeo = mmap(NULL,tamanio,PROT_READ,MAP_SHARED,descriptorDeArchivoAMapear,0);

	return mapeo;
}

void liberarMemoriaDeArchivoCompletoMapeado(FILE* archivoAMapear, void* resultadoDeMapeo){
	int tamanio, descriptorDeArchivoAMapear;

	descriptorDeArchivoAMapear = fileno(archivoAMapear);
	tamanio = tamanioDeArchivo(descriptorDeArchivoAMapear);

	munmap(resultadoDeMapeo,tamanio);
}


///////////////////////////////////////////////////


void escribirBloqueMapeado(FILE* archivo,char* contenidoAEscribir,int numDeBloque, int tamanioDeBloque){
	fseek(archivo,numDeBloque*tamanioDeBloque,SEEK_SET);

	char* aux = completarBloque(contenidoAEscribir,tamanioDeBloque);

	fwrite(aux,tamanioDeBloque,1,archivo);
}

char* leerBloqueMapeado(FILE* archivo,int numDeBloque, int tamanioDeBloque){
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




