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
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/cliente-servidor.h>


#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define ALGORITMO_PLANIFICACION "ALGORITMO_PLANIFICACION"
#define QUANTUM "QUANTUM"

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


int correr_path(void);

tipoConfigPlanificador* crearConfigPlanificador();

void destruirConfigPlanificador(tipoConfigPlanificador* estructuraDeConfiguracion);

tipoConfigPlanificador* cargarArchivoDeConfiguracionDelPlanificador(char* rutaDeArchivoDeConfiguracion);

t_PCB* PCB_create(int id, int pc, char estado, char path[30]);

int *id_create(int id);

t_CPU *cpu_create(int id_cpu, int disponibilidad, int puerto);



t_PCB* recibirPCB(int socketPlanificador);

void enviarPCB(int socketCPU,t_PCB* PCB);

#endif /* FUNCIONESPLANIFICADOR_H_ */
