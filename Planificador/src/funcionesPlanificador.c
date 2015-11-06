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


t_PCB recibirPCB2(int socketEnviador){

	size_t tamanioRuta;

	tipoPCB* respuesta = malloc(sizeof(tipoPCB));

	recibirMensajeCompleto(socketEnviador, &(respuesta->pid), sizeof(int));

	recibirMensajeCompleto(socketEnviador, &(respuesta->insPointer), sizeof(int));

	recibirMensajeCompleto(socketEnviador, &(respuesta->estado), sizeof(char));

	recibirMensajeCompleto(socketEnviador, &tamanioRuta, sizeof(size_t));

	respuesta->ruta = malloc(tamanioRuta);

	recibirMensajeCompleto(socketEnviador, respuesta->ruta, tamanioRuta);

	t_PCB pcb;


	pcb.id = respuesta->pid;
	pcb.pc = respuesta->insPointer;
	pcb.estado = respuesta->estado;
    memset(pcb.path, '\0', 30);
    strcpy(pcb.path,respuesta->ruta);


	return pcb;

}

void enviarPCB2(int socketReceptor, t_PCB PCB_leo)
{

	tipoPCB PCB;

	PCB.pid = PCB_leo.id;
	PCB.insPointer = PCB_leo.pc;
	PCB.estado = PCB_leo.estado;
	PCB.ruta =strdup(PCB_leo.path);



		size_t tamanioRuta = strlen(PCB.ruta) + sizeof(char);

		enviarMensaje(socketReceptor, &(PCB.pid), sizeof(int));

		enviarMensaje(socketReceptor, &(PCB.insPointer), sizeof(int));

		enviarMensaje(socketReceptor, &(PCB.estado), sizeof(char));

		enviarMensaje(socketReceptor, &tamanioRuta, sizeof(int));

		enviarMensaje(socketReceptor, PCB.ruta, tamanioRuta);
}

