
#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/mman.h>
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/estructuras.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/configuracion.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>


/**********Configuracion Inicial del CPU**********/
#define RUTA_DE_ARCHIVO_DE_CONFIGURACION_CPU "cfgCPU"
#define RUTA_DE_ARCHIVO_DE_LOGS_CPU "logsCPU"
/**********Fin de configuracion Inicial del CPU**********/


/**********Definiciones de CPU**********/
#define CONTINUA_EJECUCION 0
#define SALIDA_BLOQUEANTE_POR_ERROR 1
#define SALIDA_BLOQUEANTE 2
/**********Fin de Definiciones de CPU**********/

/**********Definiciones de Instrucciones de Planificador**********/
#define EXISTENCIA_DE_ARCHIVO 'A'
#define CANTIDAD_DE_INSTRUCCIONES_DE_UN_PROCESO 'C'
#define PORCENTAJE_DE_USO_DEL_CPU '%'
#define FINALIZAR_CPU 'S'
/**********Fin de Definiciones de Instrucciones de Planificador**********/


/**********Archivo de Config de CPU**********/
#define IP_PLANIFICADOR "IP_PLANIFICADOR"
#define PUERTO_PLANIFICADOR "PUERTO_PLANIFICADOR"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define CANTIDAD_HILOS "CANTIDAD_HILOS"
#define RETARDO "RETARDO"
#define METODO_PORCENTAJE_DE_USO "METODO_PORCENTAJE_DE_USO"
#define DEBUG "DEBUG"

typedef struct{
	char* ipPlanificador;
	int puertoPlanificador;
	char* ipMemoria;
	int puertoMemoria;
	int cantidadDeHilos;
	int retardo;
	int metodoPorcentajeDeUso;
	int debug;
}tipoConfigCPU;

tipoConfigCPU* cargarArchivoDeConfiguracionDeCPU(char* rutaDelArchivoDeConfiguracionDelCPU);

tipoConfigCPU* crearConfigCPU();

void destruirConfigCPU(tipoConfigCPU* estructuraDeConfiguracion);
/**********Fin de Archivo de Config de CPU**********/


/**********Semaforos del CPU**********/
sem_t semaforoConexionMasterPlanificador;
sem_t semaforoLogs;
sem_t semaforoInstruccionesCPU;
sem_t semaforoCPUTrabajando;
sem_t semaforoContadorDeInstrucciones;
/**********Fin de Semaforos Inicial del CPU**********/

/**********Lista de Instrucciones finalizadas de cada CPU**********/
t_list* cantidadDeInstruccionesEjecutadasPorCPUs;
/**********Fin de Lista de Instrucciones finalizadas de cada CPU**********/

/**********Timers de CPU**********/
time_t inicioDeProceso;
time_t finDeProceso;
/**********Fin de Timers de CPU**********/


/**********Estructuras del CPU**********/
/*Estructura de datos del CPU*/
typedef struct{
	int idCPU;
	tipoConfigCPU* configuracionCPU;
	t_log* logCPU;
	int socketParaPlanificador;
	int socketParaMemoria;
	int quantum;
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
void* unCPU(t_datosCPU*);

/*Hilo de conexion master con Planificador que atiende solicitudes de porcentaje de uso, cantidad de instrucciones y finalizar*/
void* conexionMasterPlanificador(tipoConfigCPU* configuracionCPU);

/*Hilo que reinicia los contadores de instrucciones ejecutadas por los hilos CPU cada un minuto*/
void* resetearInstruccionesDeCPUs(int* cantidadDeCPUs);

int validarExistenciaDeArchivo(char* rutaDelArchivo);
FILE* abrirProgramaParaLectura(char* rutaDelPrograma);
int longitudDeStringArray(char** stringArray);
void ejecutarPrograma(tipoPCB *PCB, t_datosCPU* datosCPU);
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

/*Funciones de Instrucciones de Planificador*/
int cantidadDeInstrucciones(char* rutaDelPrograma);
void enviarPorcentajeDeUso(int socketMasterPlanificador, tipoConfigCPU* configuracionCPU);

/*Funciones de Porcentaje de Uso*/
void asignarCantidadDeCPUsALista(int cantidadDeCPUs);
void aumentarCantidadDeInstruccionesEjecutadasEnUno(int idCPU);
void reiniciarCantidadDeInstrucciones(int cantidadDeCPUS);
/**********Fin de Funciones del CPU**********/

#endif /* FUNCIONESCPU_H_ */
