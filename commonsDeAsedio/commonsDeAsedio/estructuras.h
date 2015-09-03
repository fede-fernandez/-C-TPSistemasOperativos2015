#ifndef COMMONSDEASEDIO_ESTRUCTURAS_H_
#define COMMONSDEASEDIO_ESTRUCTURAS_H_

/*********FILESYSTEM****************/

typedef struct{
	int nombre;
	long ip;
	int puerto;
}tipoNodo;

typedef struct{
	int bloqueFS;
	tipoNodo *listaDeNodos;
}tipoBloqueNodo;

/***********FIN FILESYSTEM**************/

/*
typedef struct{
	char* nombreDeArchivo;
	int padre;
}tipoArchivo;

typedef struct{
	char* nodo;
	int numeroDeBloque;
}tipoBloque;

typedef struct{
	tipoBloque bloques[3];
};*/


#endif /* COMMONSDEASEDIO_ESTRUCTURAS_H_ */
