/*
 * estructurasPlanificador.c
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

//

#include "funcionesPlanificador.h"



tipoConfigPlanificador* crearConfigPlanificador(){
	tipoConfigPlanificador *cfg = malloc(sizeof(tipoConfigPlanificador));
	cfg->algoritmoDePlanificacion = string_new();

	return cfg;
}


void destruirConfigPlanificador(tipoConfigPlanificador* cfg){
	free(cfg->algoritmoDePlanificacion);
	free(cfg);
}

tipoConfigPlanificador* cargarArchivoDeConfiguracionDelPlanificador(char* rutaDeArchivoDeConfiguracion){
	t_config* archivoConfig = config_create(rutaDeArchivoDeConfiguracion);

	tipoConfigPlanificador* cfg = crearConfigPlanificador();

	validarErrorYAbortar(config_has_property(archivoConfig,PUERTO_ESCUCHA)
			&& config_has_property(archivoConfig,ALGORITMO_PLANIFICACION)
			&& config_has_property(archivoConfig,QUANTUM),
			"Las claves del archivo de configuracion no coinciden con las que requiere el Planificador.");

	cfg->puertoEscucha = config_get_int_value(archivoConfig,PUERTO_ESCUCHA);
	cfg->algoritmoDePlanificacion = string_duplicate(config_get_string_value(archivoConfig,ALGORITMO_PLANIFICACION));
	cfg->quantum = config_get_int_value(archivoConfig,QUANTUM);

	config_destroy(archivoConfig);

	return cfg;
}


