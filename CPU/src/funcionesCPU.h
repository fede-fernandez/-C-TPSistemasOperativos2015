
#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_

#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/estructuras.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <pthread.h>
#include <semaphore.h>

/************Archivo de Config de CPU************/
#define IP_PLANIFICADOR "IP_PLANIFICADOR"
#define PUERTO_PLANIFICADOR "PUERTO_PLANIFICADOR"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define CANTIDAD_HILOS "CANTIDAD_HILOS"
#define RETARDO "RETARDO"

#define RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU "cfgCPU"
#define RUTA_DE_ARCHIVO_DE_LOGS_CPU "logsCPU"
#define LOGS_ACTIVADOS 0
#define DEBUG 1

#define CONTINUA_EJECUCION 0
#define SALIDA_BLOQUEANTE_POR_ERROR 1
#define SALIDA_BLOQUEANTE 2


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

//Estructura Instruccion a Memoria
typedef struct{
	char* nombreDeInstruccion;
	char* valorDeInstruccion1;
	char* valorDeInstruccion2;
}t_instruccion;

//Estructura Hilo CPU
typedef struct
{
	tipoConfigCPU* configuracionCPU;
	t_log* logCPU;
}t_hiloCPU;

typedef struct
{
	int idCPU;
	tipoConfigCPU* configuracionCPU;
	t_log* logCPU;
	int socketParaPlanificador;
	int socketParaMemoria;
}t_datosCPU;

void* unCPU(t_hiloCPU*);


FILE* abrirProgramaParaLectura(char* rutaDelPrograma);
int longitudDeStringArray(char** stringArray);
int ejecutarPrograma(tipoPCB *PCB, int quantum, t_datosCPU* datosCPU);
t_instruccion extraerInstruccion(char* instruccion);
int ejecutarInstruccion(char* lineaDeInstruccion, int idDeProceso, t_datosCPU* datosCPU);
tipoRespuesta* enviarInstruccionAMemoria(int idDeProceso, char instruccion, int numeroDePagina, char* texto, t_datosCPU* datosCPU);

/*Funciones mAnsisOp*/
int instruccionIniciar(int numeroDePaginas, int idDeProceso, t_datosCPU* datosCPU);
int instruccionLeer(int numeroDePagina, int idDeProceso, t_datosCPU* datosCPU);
int instruccionEscribir(int numeroDePagina, char* textoAEscribir, int idDeProceso, t_datosCPU* datosCPU);
int instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso, t_datosCPU* datosCPU);
int instruccionFinalizar(int idDeProceso, t_datosCPU* datosCPU);


/*Funciones de control de lineas de codigo mAnsisOp*/
bool esInstruccionIniciar(char* instruccion);
bool esInstruccionLeer(char* instruccion);
bool esInstruccionEscribir(char* instruccion);
bool esInstruccionEntradaSalida(char* instruccion);
bool esInstruccionFinalizar(char* instruccion);
char* sacarComillas(char* frase);


#endif /* FUNCIONESCPU_H_ */
