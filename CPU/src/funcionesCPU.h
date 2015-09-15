
#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_

#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/error.h>

/************Archivo de Config de CPU************/
#define IP_PLANIFICADOR "IP_PLANIFICADOR"
#define PUERTO_PLANIFICADOR "PUERTO_PLANIFICADOR"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define CANTIDAD_HILOS "CANTIDAD_HILOS"
#define RETARDO "RETARDO"


typedef struct{
	char* ipPlanificador;
	int puertoPlanificador;
	char* ipMemoria;
	int puertoMemoria;
	int cantidadDeHilos;
	int retardo;
}tipoConfigCPU;


tipoConfigCPU* cargarArchivoDeConfiguracionDeCPU(char* rutaDelArchivoDeConfiguracionDelCPU);

tipoConfigCPU* crearConfigCPU();

void destruirConfigCPU(tipoConfigCPU* estructuraDeConfiguracion);
/*********Fin de Archivo de Config de CPU************/


/************Funciones principales del CPU************/
int ejecutarInstruccion(char* instruccion, int idDeProceso);

/*Funciones mAnsisOp*/
int instruccionIniciar(int numeroDePaginas, int idDeProceso);
int instruccionLeer(int numeroDePagina);
int instruccionEscribir(int numeroDePagina, char* textoAEscribir, int idDeProceso);
int instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso);
int instruccionFinalizar(int idDeProceso);
/*Fin de Funciones mAnsisOp*/
/*********Fin de Funciones principales del CPU*********/


#endif /* FUNCIONESCPU_H_ */
