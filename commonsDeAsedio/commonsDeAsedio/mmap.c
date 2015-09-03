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
	tamanio = tamanioDeArchivo(fileno(archivoAMapear));

	munmap(resultadoDeMapeo,tamanio);
}
