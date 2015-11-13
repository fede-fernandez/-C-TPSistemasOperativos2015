/*
 * estructurasPlanificador.h
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

#ifndef FUNCIONESPLANIFICADOR_H_
#define FUNCIONESPLANIFICADOR_H_

#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>


typedef struct{
	int id;
	int pc;
	char estado;
	char path[30];
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


void inicializar_semaforos();
int liberar_memoria();
void crear_lista();
void liberar_puertos();

void liberar_pcb(t_PCB *PCB);
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

void ps();
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
t_PCB* PCB_create(int id, int pc, char estado, char path[30]);

int *id_create(int id);

t_CPU *cpu_create(int id_cpu, int disponibilidad, int puerto);

t_bloqueados *bloquedito_create(int id, int tiempo);
//------------------------------------------------------------------------

//-------------------------------------------------------
int diponibilidad(t_CPU * nodo);

int buscar_por_puerto(t_CPU *nodo );
//-------------------------------------------------------

void enviarPCB2(int socketCliente,t_PCB pcb);

t_PCB recibirPCB2(int socketEnviador);


#endif /* FUNCIONESPLANIFICADOR_H_ */
