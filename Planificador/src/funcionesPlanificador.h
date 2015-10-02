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


#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define ALGORITMO_PLANIFICACION "ALGORITMO_PLANIFICACION"
#define QUANTUM "QUANTUM"

extern int puertoConCambios=0; // puerto donde hubo cambios

typedef struct{
	int puertoEscucha;
	char* algoritmoDePlanificacion;
	int quantum;
}tipoConfigPlanificador;

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


//-------Funciones HILOS prototipadas----------------

void* recibir_conexion();

int llega_quantum(t_PCB *PCB);

int llega_entrada_salida(t_PCB *PCB);

int llega_de_fin(t_PCB *PCB);

void* recibir_rafagas();

void* bloquear_procesos();

void* ejecutar_proceso();
//-------------------------------------------------------------------

int menu(void) ;

int correr_path(void);
//-------------------------------------------------------------------

tipoConfigPlanificador* crearConfigPlanificador();

void destruirConfigPlanificador(tipoConfigPlanificador* estructuraDeConfiguracion);

tipoConfigPlanificador* cargarArchivoDeConfiguracionDelPlanificador(char* rutaDeArchivoDeConfiguracion);

//-----------------------------------------------------------------------
t_PCB* PCB_create(int id, int pc, char estado, char path[30]);

int *id_create(int id);

t_CPU *cpu_create(int id_cpu, int disponibilidad, int puerto);
//------------------------------------------------------------------------

//-------------------------------------------------------
int diponibilidad(t_CPU * nodo);

int buscar_por_puerto(t_CPU *nodo );
//-------------------------------------------------------

t_PCB* recibirPCB(int socketPlanificador);

void enviarPCB(int socketCPU,t_PCB* PCB);

#endif /* FUNCIONESPLANIFICADOR_H_ */
