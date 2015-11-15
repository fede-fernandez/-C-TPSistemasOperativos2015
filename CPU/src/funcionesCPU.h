
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
#include <commonsDeAsedio/configuracion.h>
#include <pthread.h>
#include <semaphore.h>


/**********Configuracion Inicial del CPU**********/
#define RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU "cfgCPU"
#define RUTA_DE_ARCHIVO_DE_LOGS_CPU "logsCPU"
#define LOGS_ACTIVADOS 1
#define DEBUG 0
/**********Fin de configuracion Inicial del CPU**********/


/**********Definiciones de CPU**********/
#define CONTINUA_EJECUCION 0
#define SALIDA_BLOQUEANTE_POR_ERROR 1
#define SALIDA_BLOQUEANTE 2
/**********Fin de Definiciones de CPU**********/


/**********Archivo de Config de CPU**********/
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
/**********Fin de Archivo de Config de CPU**********/



/**********Estructuras del CPU**********/
/*Estructura Hilo CPU*/
typedef struct{
	tipoConfigCPU* configuracionCPU;
	t_log* logCPU;
}t_hiloCPU;

/*Estructura de datos del CPU*/
typedef struct{
	int idCPU;
	tipoConfigCPU* configuracionCPU;
	t_log* logCPU;
	int socketParaPlanificador;
	int socketParaMemoria;
}t_datosCPU;

/*Estructura Instruccion a Memoria*/
typedef struct{
	char* nombreDeInstruccion;
	char* valorDeInstruccion1;
	char* valorDeInstruccion2;
}t_instruccion;

/*Estructura de Respuesta de Instruccion enviada a Memoria*/
typedef struct respuestaDeInstruccion{
	int tipoDeSalida; //(0 = continua ejecucion | 1 = termina ejecucion mantiene insPointer | 2 = termina ejecucion aumenta insPointer)
	char* respuesta;
}tipoRepuestaDeInstruccion;
/**********Fin de Estructuras del CPU**********/



/**********Funciones del CPU**********/
/*Hilo CPU, se conecta con Planificador y Memoria, espera PCBs y los ejecuta*/
void* unCPU(t_hiloCPU*);


FILE* abrirProgramaParaLectura(char* rutaDelPrograma);
int longitudDeStringArray(char** stringArray);
void ejecutarPrograma(tipoPCB *PCB, int quantum, t_datosCPU* datosCPU);
t_instruccion extraerInstruccion(char* instruccion);
tipoRepuestaDeInstruccion ejecutarInstruccion(char* lineaDeInstruccion, int idDeProceso, t_datosCPU* datosCPU);
tipoRespuesta* enviarInstruccionAMemoria(int idDeProceso, char instruccion, int numeroDePagina, char* texto, t_datosCPU* datosCPU);

/*Funciones mAnsisOp*/
tipoRepuestaDeInstruccion instruccionIniciar(int numeroDePaginas, int idDeProceso, t_datosCPU* datosCPU);
tipoRepuestaDeInstruccion instruccionLeer(int numeroDePagina, int idDeProceso, t_datosCPU* datosCPU);
tipoRepuestaDeInstruccion instruccionEscribir(int numeroDePagina, char* textoAEscribir, int idDeProceso, t_datosCPU* datosCPU);
tipoRepuestaDeInstruccion instruccionEntradaSalida(int tiempoDeEspera, int idDeProceso, t_datosCPU* datosCPU);
tipoRepuestaDeInstruccion instruccionFinalizar(int idDeProceso, t_datosCPU* datosCPU);


/*Funciones de control de lineas de codigo mAnsisOp*/
bool esInstruccionIniciar(char* instruccion);
bool esInstruccionLeer(char* instruccion);
bool esInstruccionEscribir(char* instruccion);
bool esInstruccionEntradaSalida(char* instruccion);
bool esInstruccionFinalizar(char* instruccion);
char* sacarComillas(char* frase);
/**********Fin de Funciones del CPU**********/

#endif /* FUNCIONESCPU_H_ */
