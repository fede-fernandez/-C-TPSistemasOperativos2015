#include "funcionesSeniales.h"
#include "funcionesMemoria.h"

extern int opcionSignalElegida; // variable global, su declaracion esta en funcionesMemoria.h

void crearHijoYPadre();

void imprimirYElegirOpcionesSeniales(){
	while(true){
		//system("clear");
		printf("Menu de Administrador de Memoria\n\n");

		printf("1) Limpiar la TLB (TLB flush)\n");
		printf("2) Limpiar la Memoria Principal\n");
		printf("\n");

		printf("Elija una opcion opcion: ");
		scanf("%d",&opcionSignalElegida);

		elegirOpcion();
	}
}

void elegirOpcion(){

	switch(opcionSignalElegida){
		case 1:
			signal(SIGUSR1, limpiarTLB);
			break;

		case 2:
			signal(SIGUSR2, limpiarRam);
			break;

		default:
			system("clear");
			imprimirYElegirOpcionesSeniales();
			break;
	}
}


void tratarSenial(){

	switch(opcionSignalElegida){
		case 1:
			limpiarTLB();
			break;

		case 2:
			limpiarRam();
			break;

		case 3:
			crearHijoYPadre();
			break;
	}

	opcionSignalElegida = 0;
}

void prepararSenialLimpiarTLB(int signal){
	opcionSignalElegida = 1;
}

void prepararSenialLimpiarRAM(int signal){
	opcionSignalElegida = 2;
}

void prepararSenialVolcarRamALog(int signal){
	opcionSignalElegida = 3;
}

void crearSeniales(){

	while(1){
		signal(SIGUSR1, prepararSenialLimpiarTLB);
		signal(SIGUSR2, prepararSenialLimpiarRAM);
		signal(SIGPOLL, prepararSenialVolcarRamALog);
	}
}
















