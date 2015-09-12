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

t_estructura_PCB* PCB_create(int id, int pc, char estado[20], char path[30]) { // esta funcion crea la estructura
	t_estructura_PCB *new = malloc(sizeof(t_estructura_PCB));
    new->id = id;
    new->pc = pc;
    memset(new->estado, '\0', 20);
    strcpy(new->estado,estado);
    memset(new->path, '\0', 30);
    strcpy(new->path,path);
    return new;
}

t_estructura_PCB* recibirPCB(int socketPlanificador){

	size_t tamanioBloque = sizeof(t_estructura_PCB);

	void* bloque = malloc(tamanioBloque);

	recibirMensajeCompleto(socketPlanificador,bloque,tamanioBloque);

	t_estructura_PCB* pcbRecibido;

	memcpy(bloque,pcbRecibido->id,sizeof(int));
	memcpy(bloque+sizeof(int),pcbRecibido->pc,sizeof(int));
	memcpy(bloque+2*sizeof(int),pcbRecibido->estado,20*sizeof(char));
	memcpy(bloque+2*sizeof(int)+20*sizeof(char),pcbRecibido->path,30*sizeof(char));

	return pcbRecibido;
}

