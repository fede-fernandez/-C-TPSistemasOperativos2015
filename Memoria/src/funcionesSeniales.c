#include "funcionesSeniales.h"
#include "funcionesMemoria.h"

#include <signal.h>

extern int opcionSignalElegida; // variable global, su declaracion esta en funcionesMemoria.h

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
			//printf("\nEspero la señal para limpiar TLB\n");
			signal(SIGUSR1, limpiarTLB);
			break;

		case 2:
			//printf("\nEspero la señal para limpiar la RAM\n");
			signal(SIGUSR2, limpiarRam);
			break;

		default:
			system("clear");
			imprimirYElegirOpcionesSeniales();
			break;
	}
}


void tratarSenial(){

	//printf("\nMando la señal para que ejecute lo que eligio\n");

	switch(opcionSignalElegida){
		case 1:
			kill(getpid(), SIGUSR1);
			break;

		case 2:
			kill(getpid(), SIGUSR2);
			break;

		default:
			opcionSignalElegida = 0;
			break;
	}
}
