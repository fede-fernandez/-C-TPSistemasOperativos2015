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

void cargarArchivoDeConfiguracionDelPlanificador(t_config* archivoDeConfiguracion, tipoConfigPlanificador* estructuraDeConfiguracion);


#endif /* FUNCIONESPLANIFICADOR_H_ */
