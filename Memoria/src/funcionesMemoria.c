/*
 * funcionesMemoria.c
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */
#include "funcionesMemoria.h"

tipoConfigMemoria crearConfigMemoria(){
	tipoConfigMemoria* cfg = malloc(sizeof(tipoConfigMemoria));
	cfg->ipPlanificador = string_new();
	cfg->ipMemoria = string_new();

	return cfg;
}

void destruirConfigMemoria(tipoConfigMemoria* cfg){
	free(cfg->ipPlanificador);
	free(cfg->ipMemoria);
	free(cfg);
}

void cargarArchivoDeConfiguracionDeMemoria(t_config* archivoCfg,tipoConfigMemoria* cfg){
	if () {

	}
}
