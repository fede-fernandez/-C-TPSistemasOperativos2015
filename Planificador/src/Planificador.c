#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include "funcionesPlanificador.h"
#include <sys/socket.h>
#include <sys/types.h>

#include <commonsDeAsedio/cliente-servidor.h>



// VARIABLES GLOBALES <-------------------

int contador_de_id_procesos = 0; // para saber cuantos procesos hay en el sistema
t_list *lista_de_PCB;
t_queue *procesos_en_ready; // cola de procesos "listos para ejecutar"
t_list *CPUs; // lista de cpu disponibles
t_queue *procesos_bloqueados; // cola donde se encolan los procesos para luego mandarlos a dormir
int socketEscucha; // socket que escuche las conecciones entrantes
int quantum; // 0 si tiene quantum y el "valor" en caso de que tenga quantum
fd_set master; // conjunto maestro de descriptores de fichero
fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
int fdmax; // número máximo de descriptores de fichero

int puerto = 7200;//Elijo 7200 pero esto se carga del archivo de configuracion

//--------------------------------------------------------------------------------------------------

//Las movi al header las estructuras y funciones_create()

// -------------------------------------------------------------------------------------------------------


//-------------------------------------FUNCIONES HILOS--------------------------------------------------------------------



//---------------HILO encargado de recibir conexiones de CPUs   -------------------------

void* recibir_conexion(void){


	int socketCpu;
	int id;

	socketEscucha = crearSocket();
	asociarAPuerto(socketEscucha,puerto);
	escucharConexiones(socketEscucha,1); //  me pongo a escuchar conexiones

	while(1){

		socketCpu = crearSocketParaAceptarSolicitudes(socketEscucha); // es bloqueante ?¿?,

		recibirMensaje(socketCpu, &id, sizeof(int));// recibo id de CPU

		list_add(CPUs, cpu_create(id,1,socketCpu)); // guardo en la lista

		enviarMensaje(socketCpu,&quantum,sizeof(quantum)); // le mando el quantum, que es un int

		FD_SET(socketCpu, &master); // Agrega socketCpu al master set
		if (socketCpu > fdmax) { // es el mayor
			fdmax = socketCpu; // guardo el mayor
		}

	}

	return 0;
}



//-----------------------------------------------------------------------------------------------------------



//------- HILO encargado de recibir las rafagas de las CPU que vienen de: quantum/entrada_salida/fin---------

int llega_quantum(t_PCB *PCB){

	// meter procesos en la cola de ready
	queue_push(procesos_en_ready,id_create(PCB->id));

	// actualizo el PCB

	PCB->estado = 'R'; // le cambio el valor que esta en memoria dinamica

	return 0;

}

int llega_entrada_salida(t_PCB *PCB){

	// meter procesos en la cola de ready
	queue_push(procesos_bloqueados,id_create(PCB->id ));

	// actualizo el PCB

	PCB->estado = 'B'; // le cambio el valor que esta en memoria dinamica

	return 0;

}

int llega_de_fin(t_PCB *PCB){

	PCB->estado = 'F'; // le cambio el valor que esta en memoria dinamica

	return 0;

}

void* recibir_rafagas(void){

	t_CPU *nodo_cpu;
	t_PCB *PCB_recibido;
	t_PCB *PCB;
	int id_cpu;
	char llegada; // "Quantum", "Bloqueado" y "Fin"
	int i=0; // puerto donde hubo cambios



	while(1){

		read_fds = master; // backup de mi descriptores

		// select se bloque hasta que le llegan "mensajitos"
		select(fdmax+1, &read_fds, NULL, NULL, NULL);
		for(i = 0; i <= fdmax; i++) {

			// preguntar a todos los puertos de "read_fds" si recibieron mensajes
			if (FD_ISSET(i, &read_fds)) { //pregunta si i está en el conjunto y si hubo cambio

				break; // encontramos el puerto donde hubo cambios, "i" es el puerto con cambios
				// podria poner un goto xDDD (para q salga del for) xDDD
			}
		}

		//una vez que encontramos el puerto, lo saco con "i" al nodo de la lista

		nodo_cpu=buscar_nodo_por_puerto(CPUs,i); // falta desarrollar esa funcion de busquedad

		// llegada es un protocolo de comunicacion, para saber que hacer con el PCB del proceso llegante

		recibirMensaje(nodo_cpu->puerto, &llegada, sizeof(char));// recibo llegada

		PCB_recibido = recibirPCB(nodo_cpu->puerto); // recibe el PCB

		// buscar id de proceso en "lista_de_PCB"

		PCB =list_get(lista_de_PCB, PCB_recibido->id - 1);

		*PCB = *PCB_recibido; // actualizo PCB ---> la magia de c =)



		switch (llegada) {
		  case   'Q':
		      llega_quantum(PCB);	       break; // va a meter ese proceso a la cola de redy y actualizar PCB
		  case   'B':
			  llega_entrada_salida(PCB);   break; // va a meter ese proceso a la cola de Entara-Salida, para despues bloquearlo y actualizar PCB
		  case   'F':
		      llega_de_fin(PCB);	       break; // unicamente actualiza el PCB del proceso llegante

		}

		// agregar esa CPU como disponible();
		nodo_cpu->disponibilidad = 1;

		// despertar al hilo ejecutar_proceso();

		free(PCB_recibido);

	}

}




//-------------------------------------------------------------------------------------------------------------



//---------------HILO encargado de dormir procesesos durante un cierto tiempo "T"--------------------------
//---------------tambien los pasa a la cola de "listos" una vez pasado ese tiempo-------------------------


void* bloquear_procesos(void){

	t_bloqueados *nodo_bloqueado;
	t_PCB *nodo_pcb;


	while(1){

		// sacar un proceso de la cola de "procesos_bloqueados" (modificando la lista)

		nodo_bloqueado = queue_pop(procesos_bloqueados);

		sleep(nodo_bloqueado->tiempo);

		// una vez transcurrido ese tiempo buscar con la id en "lista_de_PCB" y sacar su PCB (sin modificar la lista)

		nodo_pcb =list_get(lista_de_PCB, nodo_bloqueado->id - 1); // se modifica la lista ?? (no se tiene q modificar!)

		// modificar su estado de: bloqueado a--> listo y meter de nuevo en "lista_de_PCB"

		nodo_pcb->estado = 'R'; // le cambio el valor que esta en memoria dinamica

		// meter id de proceso en cola: "procesos_en_ready"

		queue_push(procesos_en_ready,nodo_bloqueado->id); // ya hay un nuevo nodo en la cola

		free(nodo_bloqueado); // saco el nodo de memoria dinamica


	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------

//---------------HILO encargado de mandar a ejecutar procesos a la CPUs-------------------------

void* ejecutar_proceso(void){

	int *id;
	t_PCB *nodo_pcb;
	t_CPU * nodo_cpu;

	while(1){

		// wait(cant_procesos_listos);

		// wait(cant_CPUs_libres);

		id = queue_pop(procesos_en_ready); //sacar_primer_elemento_de_la_cola

		nodo_pcb =list_get(lista_de_PCB, *id-1); //PCB=buscar_id_de_proceso (sin desarmar la lista)

		nodo_pcb->estado = 'E'; // le cambio el valor que esta en memoria dinamica

		//buscar_CPU_disponible.Esta funcion me devuelve un puerto libre
		nodo_cpu=list_find(CPUs,(void*)diponibilidad); // siempre tiene q haber un puerto libre, o explota el programa

		// mandar_PCB_al_cpu();

		free(id);


	}

//--------------------------------------------------------------------------------------------------------

int menu(void) {

	int opcion;

	while(1) // el menu tiene que estar presente siempre

	{
		 // limpiar pantalla

		opcion = 0;

			printf("################################################################\n");
			printf("#     --------> **************************** <-------------    #\n");
			printf("#     --------> *****  LOS  JAVIMANCOS ***** <-------------    #\n");
			printf("#--------------------------------------------------------------#\n");
			printf("#                                                              #\n");
			printf("#  Ingrese una opción para continuar:                          #\n");
			printf("#                                                              #\n");
			printf("#                                                              #\n");
			printf("#     1) Correr Path                                           #\n");
			printf("#                                                              #\n");
			printf("#     2) Finalizar PID                                         #\n");
			printf("#                                                              #\n");
			printf("#     3) ps                                                    #\n");
			printf("#                                                              #\n");
			printf("#     4) cpu                                                   #\n");
			printf("#                                                              #\n");
			printf("################################################################\n");
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
			default: printf("Opción incorrecta. Por favor ingrese una opción del 1 al 4 \n"); break;
		  }

	}

 return 0;

}

int main(void) {

	tipoConfigPlanificador* configuracion = cargarArchivoDeConfiguracionDelPlanificador("/home/utnso/Escritorio/cfgPlanificador");

	puerto = configuracion->puertoEscucha;
	quantum = configuracion->quantum;

	destruirConfigPlanificador(configuracion);


	lista_de_PCB = list_create(); //Crea la lista_de_PCB
	procesos_en_ready = queue_create(); //Crea la cola de pocesos en ready
	CPUs = list_create(); // crea lista de CPUs conectadas
	procesos_bloqueados = queue_create(); // crea cola de procesos bloqueados

	pthread_t escucha; //Hilo que va a manejar las conecciones de las distintas CPU
	pthread_t ejecucion; //Hilo que va a mandar a ejecutar "procesos listos" a distintas CPUs
	pthread_t recibir;
	pthread_t bloquear; // hilo que manda a dormir procesos que estan en la lista de "procesos_bloqueados"

	//Este hilo va a escuchar y aceptar las conexiones, con las CPU de forma paralela a la ejecucion de este proceso "main"
	pthread_create(&escucha, NULL, recibir_conexion, NULL); // falta implementar la funcion "recibir_conexion"
	pthread_create(&ejecucion, NULL, ejecutar_proceso, NULL); // falta implementar la funcion "ejecutar_proceso"
	pthread_create(&recibir, NULL, recibir_rafagas, NULL); // falta implementar la funcion
	pthread_create(&bloquear, NULL, bloquear_procesos, NULL); //falta implementar la funcion


	menu();


	//destruir hilos
	//destruir listas..todo lo q este en memoria dinamica.

	return EXIT_SUCCESS;
}


int correr_path(void){


	char comando[30];
	char path[30];
	t_PCB *PCB;

  //limpiar pantalla

	system("clear");

	printf("Ingresar Comando: \n");

	scanf("%s %s", comando, path); // supongo que siempre es un comando valido y path tambien

	contador_de_id_procesos++; // mantengo la cuenta de todos los procesos que se crearon en el sistema

	// Agrego el elemento al final de la lista (para que quede ordenada por ID)
	list_add(lista_de_PCB, PCB_create(contador_de_id_procesos, 1, 'R', path));

	// agrego la id a lo ultimo de la cola
	queue_push(procesos_en_ready,id_create(contador_de_id_procesos));

	printf("Proceso %s en ejecucion....\n", path);

	sleep(2);

	return 0;
}
