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

 t_PCB *PCB_create(int id, int pc, char estado, char path[30]) { // esta funcion crea la estructura
	t_PCB *new = malloc(sizeof(t_PCB));
    new->id = id;
    new->pc = pc;
    new->estado = estado;
    memset(new->path, '\0', 30);
    strcpy(new->path,path);
    return new;
}

 t_CPU *cpu_create(int id_cpu, int disponibilidad, int puerto) { // esta funcion crea la estructura
	t_CPU *new = malloc(sizeof(t_CPU));
    new->id_cpu = id_cpu;
    new->disponibilidad = disponibilidad;
    new->puerto = puerto;
    return new;
}

 int *id_create(int id){
	int*new=malloc(sizeof(int));
	*new=id;
	return new;

}

 t_bloqueados *bloquedito_create(int id, int tiempo){

		t_bloqueados *new = malloc(sizeof(t_bloqueados));
	    new->id = id;
	    new->tiempo = tiempo;

	    return new;
 }

 int diponibilidad(t_CPU * nodo){ // condicion para encontrar un puerto disponible

 	if(nodo->disponibilidad == 1){
 		return 1;
 	}
 	else{
 		return 0;
 	}
 }


void liberar_pcb(t_PCB *PCB){

	free(PCB);

}


void enviarPCB2(int socketReceptor,t_PCB pcb){

	enviarMensaje(socketReceptor,&pcb.id,sizeof(int));
	enviarMensaje(socketReceptor,&pcb.pc,sizeof(int));
	enviarMensaje(socketReceptor,&pcb.estado,sizeof(char));
	enviarMensaje(socketReceptor,&pcb.path,sizeof(char[30]));

}

t_PCB recibirPCB2(int socketEnviador){

	t_PCB pcb;
	int id;
	int pc;
	char estado;
	char path[30];

	recibirMensaje(socketEnviador, &id, sizeof(int));
	recibirMensaje(socketEnviador, &pc, sizeof(int));
	recibirMensaje(socketEnviador, &estado, sizeof(char));
	recibirMensaje(socketEnviador, &path, sizeof(path));

	pcb.id = id;
	pcb.pc = pc;
	pcb.estado = estado;
    memset(pcb.path, '\0', 30);
    strcpy(pcb.path,path);

	return pcb;
}

