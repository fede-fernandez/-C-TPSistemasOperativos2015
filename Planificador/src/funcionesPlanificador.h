/*
 * estructurasPlanificador.h
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

#ifndef PLANIFICADOR_SRC_FUNCIONESPLANIFICADOR_H_
#define PLANIFICADOR_SRC_FUNCIONESPLANIFICADOR_H_

#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>
#include <commonsDeAsedio/configuracion.h>
#include <time.h>


typedef struct{
	int id;
	int pc;
	char estado;
	char path[30];
	time_t ultimaRespuesta; //El tiempo real desde que empieza, hasta qué termina
	double tiempoEjecucion; // La suma de los periodos de tiempo en la CPU
	time_t ultimaEjecucion;
	double tiempoEspera; //La suma de los periodos de tiempo en la cola de listos
	time_t ultimaEspera;
} t_PCB;

typedef struct{
	int id_cpu;
	int disponibilidad;// "0" NO Esta disponible y, "1" Esta disponible
	int puerto;
} t_CPU;

typedef struct{
	int id; // id proceso
	int tiempo;// tiempo que se va a dormir el procesos
} t_bloqueados;

//FERNILANDIA
#define NEGRO "\033[1m\033[30m"
#define ROJO "\033[1m\033[31m"
#define VERDE "\033[1m\033[32m"
#define AMARILLO "\033[1m\033[33m"
#define AZUL "\033[1m\033[34m"
#define BLANCO "\033[1m\033[37m"
#define FINDETEXTO "\033[0m"
//FIN DE FERNILANDIA


// funciones basicas -------------------------------------

void inicializar_semaforos();
int liberar_memoria();
void crear_lista();
void liberar_puertos();

void liberar_pcb(t_list*lista_de_PCB, t_PCB *PCB);

void liberar_PCBs();
//------Funciones HILOS prototipadas----------------


void* recibir_cpu();

int recibir_conexion();

int recibir_rafagas();

int llega_quantum(t_PCB *PCB);

int llega_entrada_salida(t_PCB *PCB,int tiempo);

int llega_de_fin(t_PCB *PCB);



void* bloquear_procesos();

void* ejecutar_proceso();
//-------------------------------------------------------------------

int menu(void) ;

int correr_path(void);

int finalizar_PID();

void ps();

void cpu();
//-------------------------------------------------------------------

/************Archivo de Config de PLANIFICADOR************/

void configurar();

#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define QUANTUM "QUANTUM"

typedef struct{
	int puertoPlanificador;
	int quantum;
}tipoConfig;

tipoConfig* cargarArchivoDeConfiguracion(char* rutaDelArchivoDeConfiguracion);

tipoConfig* crearConfig();

void destruirConfig(tipoConfig* estructuraDeConfiguracion);
/*********Fin de Archivo de Config de PLANIFICADOR************/

//-----------------------------------------------------------------------
t_PCB* PCB_create(int id, int pc, char estado, char path[30],time_t ultimaRespuesta, time_t ultimaEspera);

int *id_create(int id);

t_CPU *cpu_create(int id_cpu, int disponibilidad, int puerto);

t_bloqueados *bloquedito_create(int id, int tiempo);
//------------------------------------------------------------------------

//-------------------------------------------------------

t_PCB *buscar_PCB(t_list*lista_de_PCB, int pidProcesoBuscado);

int diponibilidad(t_CPU * nodo);

int buscar_por_puerto(t_CPU *nodo );

// saber si un mProc fue finalizado forsozamente
int estas_finalizado(t_list *id_finalizados, int pidProcesoBuscado); //

//-------------------------------------------------------

void enviarPCB2(int socketCliente,t_PCB pcb);

t_PCB recibirPCB2(int socketEnviador);

void enviarPath(int socketMaestro,t_PCB *pcb);

//---Logueo----------------------------------

void logueo();

void log_actividad_cpus(int id_cpu, char estado[15],int id_proceso,char path[30], char estado_pcb[15]);

void log_colas(int id_proceso,char path[30],char razon[40],int numero);

void log_rafagas(int sockCpu);


#endif /* PLANIFICADOR_SRC_FUNCIONESPLANIFICADOR_H_ */
