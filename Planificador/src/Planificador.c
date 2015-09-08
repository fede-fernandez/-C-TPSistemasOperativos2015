#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include "funcionesPlanificador.h"

//--------------- -------estructuras, esto va en un t_estructuras_PCB------------------------------------------

typedef struct{
	int id;
	int pc;
	char estado[20];
	char path[30];
} t_estructura_PCB;

static t_estructura_PCB *PCB_create(int id, int pc, char estado[20], char path[30]) { // esta funcion crea la estructura
	t_estructura_PCB *new = malloc(sizeof(t_estructura_PCB));
    new->id = id;
    new->pc = pc;
    memset(new->estado, '\0', 20);
    strcpy(new->estado,estado);
    memset(new->path, '\0', 30);
    strcpy(new->path,path);
    return new;
}


struct estructura_CPU{
	int id_cpu;
	int disponibilidad; // "0" NO Esta disponible y, "1" Esta disponible
};



struct estructura_CPU{
	int id_cpu;
	int disponibilidad;// "0" NO Esta disponible y, "1" Esta disponible
	int puerto;
};

// -------------------------------------------------------------------------------------------------------


// variables globales

int contador_de_id_procesos = 0; // para saber cuantos procesos hay en el sistema
t_list* lista_de_PCB;
t_list* procesos_en_ready; // lista de procesos "listos para ejecutar"


int correr_path(void);

int menu(void) {

	while(1) // el menu tiene que estar presente siempre

	{
		 //limpiar pantalla

		 int opcion = 0;

			printf("################################################################\n");
			printf("# Ingrese una opción para continuar:                           #\n");
			printf("#  1) Correr Path                                              #\n");
			printf("#  2) Finalizar PID                                            #\n");
			printf("#  3) ps                                                       #\n");
			printf("#  4) cpu                                                      #\n");
			printf("################################################################\n");

		 scanf("%d",&opcion);

		 switch (opcion) {
			case 1:
			   correr_path();	break; // las demas funciones las puedo desarrollar a lo ultimo
			/*case 2:
			   finalizar_PID(); break;
			case 3:
			   ps();	        break;
			case 4:
			   cpu();	        break;*/
			default: printf("Opción incorrecta. Por favor ingrese una opción del 1 al 4\n"); break;
		  }

	}

 return 0;

}

int main(void) {

	tipoConfigPlanificador* configuracion = cargarArchivoDeConfiguracionDelPlanificador("/home/utnso/Escritorio/cfgPlanificador");


//
	lista_de_PCB = list_create(); //Crea la lista_de_PCB
	procesos_en_ready = list_create(); //Crea la lista de pocesos en ready

	pthread_t escucha; //Hilo que va a manejar las conecciones de las distintas CPU
	pthread_t ejecucion; //Hilo que va a mandar a ejecutar "procesos listos" a distintas CPUs

	//Este hilo va a escuchar y aceptar las conexiones, con las CPU de forma paralela a la ejecucion de este proceso
	pthread_create(&escucha, NULL, conecciones_a_CPUs, NULL) // falta implementar la funcion "conecciones_a_CPUs"
	pthread_create(&ejecucion, NULL, ejecutar_proceso, NULL) // falta implementar la funcion "ejecutar_proceso"
	// aca despues voy a poner mas hilos



	menu();


	destruirConfigPlanificador(configuracion);

	return EXIT_SUCCESS;
}


int correr_path(void){
	char comando[30];
	char path[30];
	t_estructura_PCB *PCB;

  //limpiar pantalla
	printf("Ingresar Comando: \n");
	scanf("%s %s", comando, path); // supongo que siempre es un comando valido y path tambien
	contador_de_id_procesos++;
	// Agrego el elemento al final de la lista (para que quede ordenada por ID)
	list_add(lista_de_PCB, PCB_create(contador_de_id_procesos, 1, "nuevo", path));
	// agrego la id a lo ultimo de la lista
	list_add(procesos_en_ready,&contador_de_id_procesos);

	printf("Proceso %s en ejecucion....\n", parametro);

	sleep(2);

	return 0;

	}
