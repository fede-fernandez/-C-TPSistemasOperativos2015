#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include "funcionesPlanificador.h"
#include <sys/socket.h>
#include <sys/types.h>

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
	int disponibilidad;// "0" NO Esta disponible y, "1" Esta disponible
	int puerto;
};

// -------------------------------------------------------------------------------------------------------


//-------------------------------------FUNCIONES HILOS--------------------------------------------------------------------

//---------------Hilo encargado de mandar a ejecutar procesos a la CPUs-------------------------

void* ejecutar_proceso(void){

	while(1){

		// wait(cant_procesos_listos);
		// wait(cant_CPUs_libres);
		//sacar_ultimo_elemento_de_la_lista(procesos_en_ready,id);
		//PCB=buscar_id_de_proceso(lista_de_PCB,id);
		//actualizar_PCB(PCB)
		//buscar_CPU_disponible(cpu_disponibles,cpu_puerto)
		//send(cpu_puerto, PCB, strlen(PCB) + 1, 0);

	}

	return 0;
}
////..............................................................................................

//-------------- Hilo encargado de recibir las rafagas de las CPU que vienen de: quantun/entrada_salida---------

void* recibir_rafagas(void){

	while(1){
		// select se bloque hasta que le llegan "mensajitos"
		// preguntar a todos los puertos de "lista_de_PCB" si recibieron mensajes
		//una vez que encontramos el puerto, lo saco ese puerto de la lista
		// CPU = list_get(cpu_disponibles,cpu_puerto);
		// llegada es un protocolo de comunicacion, para saber que hacer con el PCB del proceso llegante
		// llegada es un protocolo de comunicacion, para saber que hacer con el PCB del proceso llegante

	    /*llegada = recv(cpu_puerto, (void*) package, PACKAGESIZE, 0); // la CPU me manda un chart: Quantum / E_S / Fin

	      PCB = recv(cpu_puerto, (void*) package, PACKAGESIZE, 0); // recibe la estructura de PCB que le manda la CPU

			    switch (llegada) {

				  case   'Q':
				      quantum(PCB);	        break; // va a meter ese proceso a la cola de redy y actualizar PCB
				  case   'B':
				      entrada_salida(PCB);      break; // va a meter ese proceso a la cola de Entara-Salida, para despues bloquearlo y actualizar PCB
				  case   'F':
				      fin(PCB);	                break; // unicamente actualiza el PCB del proceso llegante
			 }

		// agregar esa CPU como disponible(); */

	}


	return 0;
}
//-------------------------------------------------------------------------------------------------------------



//---------------Hilo encargado de recibir conexiones de CPUs   -------------------------

void* recibir_conexion(void){

	// socketEscucha = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	// bind(socketEscucha,serverInfo->ai_addr, serverInfo->ai_addrlen);
	// listen(socketEscucha, BACKLOG);

	while(1){

		//socketCPU = accept(socketEscucha, (struct sockaddr *) &addr, &addrlen);
		//idCPU = recv(socketCPU, (void*) package, PACKAGESIZE, 0); // recibe la id del CPU
		//armar nodo y meterla en la lista de CPUs , ordenado por id de cpu
		// chequear en el archivo de configuracion si el algoritmos es con quantum
		// le emando mensaje para indicar si el algortmo es con quantum / o no
		//send(socketCPU, quantum, strlen(message) + 1, 0)

	}

	return 0;
}
////..............................................................................................

//-----------------------------------------------------------------------------------------------------------


// variables globales

int contador_de_id_procesos = 0; // para saber cuantos procesos hay en el sistema
t_list* lista_de_PCB;
t_list* procesos_en_ready; // lista de procesos "listos para ejecutar"
t_list* cpu_disponibles; // lista de cpu disponibles
int socketEscucha; // socket que escuche las conecciones entrantes

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

	lista_de_PCB = list_create(); //Crea la lista_de_PCB
	procesos_en_ready = list_create(); //Crea la lista de pocesos en ready
	cpu_disponibles = list_create(); // crea lista de CPUs conectadas

	pthread_t escucha; //Hilo que va a manejar las conecciones de las distintas CPU
	pthread_t ejecucion; //Hilo que va a mandar a ejecutar "procesos listos" a distintas CPUs
	pthread_t recibir;

	//Este hilo va a escuchar y aceptar las conexiones, con las CPU de forma paralela a la ejecucion de este proceso "main"
	pthread_create(&escucha, NULL, recibir_conexion, NULL); // falta implementar la funcion "recibir_conexion"
	pthread_create(&ejecucion, NULL, ejecutar_proceso, NULL); // falta implementar la funcion "ejecutar_proceso"
	pthread_create(&recibir, NULL, recibir_rafagas, NULL); // falta implementar la funcion


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

	printf("Proceso %s en ejecucion....\n", path);

	sleep(2);

	return 0;

	}
