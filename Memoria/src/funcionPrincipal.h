/*
 * funcionPrincipal.h
 *
 *  Created on: 30/11/2015
 *      Author: utnso
 */

#ifndef FUNCIONPRINCIPAL_H_
#define FUNCIONPRINCIPAL_H_
#include "funcionesMemoria.h"

extern fd_set listaFiltrada;
extern fd_set listaPrincipal;
//extern struct timeval tiempoDeEsperaDeCpus;
extern pthread_mutex_t mutexTurnoSenial;

void funcionPrincipal(){

	while(*(datosMemoria->memoriaActiva)){
		listaFiltrada = listaPrincipal;
		select(datosMemoria->maximoSocket+1,&listaFiltrada,NULL,NULL,/*&tiempoDeEsperaDeCpus);//*/NULL);

		int var;
		for (var = 0; var <= datosMemoria->maximoSocket; var++) {

			if(FD_ISSET(var, &listaFiltrada)){

				if(var==datosMemoria->socketCpus){
					int socketCpuEntrante = crearSocketParaAceptarSolicitudes(datosMemoria->socketCpus);
					FD_SET(socketCpuEntrante,&listaPrincipal);
					datosMemoria->maximoSocket = maximoEntre(datosMemoria->maximoSocket,socketCpuEntrante);
				}
				else {
					bloquearRecurso(&mutexTurnoSenial);
					tratarPeticion(var);
					liberarRecurso(&mutexTurnoSenial);
				}
			}
		}

	}
}

#endif /* FUNCIONPRINCIPAL_H_ */
