
#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_

#include <commons/string.h>
#include <commons/config.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/estructuras.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <pthread.h>

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
int ejecutarPrograma(tipoPCB *PCB, int quantum, int tiempoDeRetardo, int socketParaPlanificador, int socketParaMemoria);
FILE* abrirProgramaParaLectura(char* rutaDelPrograma);
int ejecutarInstruccion(char* instruccion, int idDeProceso, int socketParaPlanificador, int socketParaMemoria);
int longitudDeStringArray(char** stringArray);
tipoRespuesta* enviarInstruccionAMemoria(int idDeProceso, char instruccion, int numeroDePagina, char* texto);


/*Funciones mAnsisOp*/
int instruccionIniciar(int numeroDePaginas, int idDeProceso, int socketParaMemoria);
int instruccionLeer(int numeroDePagina, int idDeProceso, int socketParaMemoria);
int instruccionEscribir(int numeroDePagina, char* textoAEscribir, int idDeProceso, int socketParaMemoria);
int instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso, int socketParaPlanificador);
int instruccionFinalizar(int idDeProceso, int socketParaPlanificador, int socketParaMemoria);


/*Funciones de control de lineas de codigo mAnsisOp*/
bool esInstruccionIniciar(char* instruccion);
bool esInstruccionLeer(char* instruccion);
bool esInstruccionEscribir(char* instruccion);
bool esInstruccionEntradaSalida(char* instruccion);
bool esInstruccionFinalizar(char* instruccion);
char* sacarComillas(char* frase);


#endif /* FUNCIONESCPU_H_ */
