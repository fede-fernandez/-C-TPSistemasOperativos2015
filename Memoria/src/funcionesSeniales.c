#include "funcionesSeniales.h"
#include "funcionesMemoria.h"

//extern int opcionSignalElegida;
extern t_list* listaSeniales;
extern void crearHijoYPadre();

/*
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
*/

void tratarSenial(){

	int tamanioListaSeniales = list_size(listaSeniales);

	if (tamanioListaSeniales== 0) return;

	int* opcionSignalElegida;

	int var;
	for (var = 0; var < tamanioListaSeniales; ++var) {

		opcionSignalElegida = list_get(listaSeniales,0);

			switch(*opcionSignalElegida){
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

			list_remove_and_destroy_element(listaSeniales, 0, free);
	}
		//*opcionSignalElegida = 0;
}

void prepararSenialLimpiarTLB(int signal){
	agregarSenialEnLaLista(1);
	//opcionSignalElegida = 1;
}

void prepararSenialLimpiarRAM(int signal){
	agregarSenialEnLaLista(2);
	//opcionSignalElegida = 2;
}

void prepararSenialVolcarRamALog(int signal){
	agregarSenialEnLaLista(3);
	//opcionSignalElegida = 3;
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
















