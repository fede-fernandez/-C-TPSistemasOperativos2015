#include "funcionesSeniales.h"
#include "funcionesMemoria.h"
#include <commonsDeAsedio/semaforos.h>

extern void crearHijoYPadre();

void tratarSenial(int signal){

	switch(signal){
		case SIGUSR1:
			liberarRecurso(&mutexLimpiarTLB);
			break;

		case SIGUSR2:
			liberarRecurso(&mutexLimpiarRam);
			break;

		case SIGPOLL:
			liberarRecurso(&mutexDump);
			break;
	}
}

void prepararSenialLimpiarTLB(){
	while(1){
		bloquearRecurso(&mutexLimpiarTLB);

		bloquearRecurso(&mutexTurnoSenial);
		limpiarTLB();
		liberarRecurso(&mutexTurnoSenial);
	}
}

void prepararSenialLimpiarRAM(){
	while(1){
		bloquearRecurso(&mutexLimpiarRam);

		bloquearRecurso(&mutexTurnoSenial);
		limpiarRam();
		liberarRecurso(&mutexTurnoSenial);
	}
}

void prepararSenialVolcarRamALog(){
	while(1){
		bloquearRecurso(&mutexDump);

		bloquearRecurso(&mutexTurnoSenial);
		crearHijoYPadre();
		liberarRecurso(&mutexTurnoSenial);
	}
}

void agregarSenialEnLaLista(int signal){
	int* opcionSignalElegida = malloc(sizeof(int));
		*opcionSignalElegida = signal;

	list_add(listaSeniales, opcionSignalElegida);
}

void crearSeniales(){

	while(1){
		signal(SIGUSR1, prepararSenialLimpiarTLB);
		signal(SIGUSR2, prepararSenialLimpiarRAM);
		signal(SIGPOLL, prepararSenialVolcarRamALog);
	}
}
















