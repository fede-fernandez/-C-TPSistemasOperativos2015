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


#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define ALGORITMO_PLANIFICACION "ALGORITMO_PLANIFICACION"
#define QUANTUM "QUANTUM"

typedef struct{
	int puertoEscucha;
	char* algoritmoDePlanificacion;
	int quantum;
}tipoConfigPlanificador;



tipoConfigPlanificador* crearConfigPlanificador();

void destruirConfigPlanificador(tipoConfigPlanificador* estructuraDeConfiguracion);

tipoConfigPlanificador* cargarArchivoDeConfiguracionDelPlanificador(char* rutaDeArchivoDeConfiguracion);


#endif /* FUNCIONESPLANIFICADOR_H_ */
