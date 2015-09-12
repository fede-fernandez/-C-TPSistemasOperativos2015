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
	char estado[20];
	char path[30];
} t_estructura_PCB;

typedef struct{
	int id_cpu;
	int disponibilidad;// "0" NO Esta disponible y, "1" Esta disponible
	int puerto;
} t_estructura_CPU;

typedef struct{
	int id;
	int tiempo;// tiempo que se va a dormir el procesos
} t_estructura_bloqueados;

t_estructura_PCB* PCB_create(int id, int pc, char estado[20], char path[30]);

tipoConfigPlanificador* crearConfigPlanificador();

void destruirConfigPlanificador(tipoConfigPlanificador* estructuraDeConfiguracion);

tipoConfigPlanificador* cargarArchivoDeConfiguracionDelPlanificador(char* rutaDeArchivoDeConfiguracion);

t_estructura_PCB* recibirPCB(int socketPlanificador);

#endif /* FUNCIONESPLANIFICADOR_H_ */
