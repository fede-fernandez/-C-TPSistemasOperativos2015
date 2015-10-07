#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "funcionesPlanificador.h"
#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>




// VARIABLES GLOBALES <-------------------

int contador_de_id_procesos = 0; // para saber cuantos procesos hay en el sistema
int quantum = 0; // 0 si tiene quantum y el "valor" en caso de que tenga quantum

//---listas y colas

t_list *lista_de_PCB;
t_queue *procesos_en_ready; // cola de procesos "listos para ejecutar"
t_list *CPUs; // lista de cpu disponibles
t_queue *procesos_bloqueados; // cola donde se encolan los procesos para luego mandarlos a dormir

//----- sockets

int puerto = 7200;//Elijo 7200 pero esto se carga del archivo de configuracion
int socketEscucha; // socket que escuche las conecciones entrantes
fd_set master; // conjunto maestro de descriptores de fichero
fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
int fdmax = 0; // número máximo de descriptores de fichero



//semaforos Mutex---------------

pthread_mutex_t pcbs;
pthread_mutex_t cpuss;
pthread_mutex_t ready;
pthread_mutex_t bloqueados;

//semaforos de sincronizacion

sem_t solicitud_ejecucion; // cantidad de solicitud_ejecucion
sem_t solicitud_cpuLibre; // cantidad de solicitud_cpuLibre
sem_t solicitud_deBloqueo; // cantidad de solicitud_deBloqueo

//--------------------------------------------------------------------------------------------------
//----Hilos
pthread_t ejecucion; //Hilo que va a mandar a ejecutar "procesos listos" a distintas CPUs
pthread_t recibir;
pthread_t bloquear; // hilo que manda a dormir procesos que estan en la lista de "procesos_bloqueados"

// -------------------------------------------------------------------------------------------------------



int main(void) {


	/*tipoConfigPlanificador* configuracion = cargarArchivoDeConfiguracionDelPlanificador("/home/utnso/Escritorio/cfgPlanificador");

	puerto = configuracion->puertoEscucha;
	quantum = configuracion->quantum;

	destruirConfigPlanificador(configuracion); */

	inicializar_semaforos();

	crear_lista();

	//Este hilo va a escuchar y aceptar las conexiones, con las CPU de forma paralela a la ejecucion de este proceso "main"
	pthread_create(&ejecucion, NULL, ejecutar_proceso, NULL); // falta testear la funcion "ejecutar_proceso"
	pthread_create(&recibir, NULL, recibir_cpu, NULL); // falta testear la funcion "recibir_rafagas"
	pthread_create(&bloquear, NULL, bloquear_procesos, NULL); //falta testear la funcion "bloquear_procesos"


	menu();

	liberar_memoria();

	return 0;
}

int correr_path(void){


	char comando[30];
	char path[30];


  //limpiar pantalla
	//system("clear");

	printf("Ingresar Comando: \n");

	scanf("%s %s", comando, path); // supongo que siempre es un comando valido y path tambien

	contador_de_id_procesos++; // mantengo la cuenta de todos los procesos que se crearon en el sistema

	pthread_mutex_lock(&pcbs);
	// Agrego el elemento al final de la lista (para que quede ordenada por ID)
	list_add(lista_de_PCB, PCB_create(contador_de_id_procesos, 1, 'R', path));

	pthread_mutex_unlock(&pcbs);

	pthread_mutex_lock(&ready);
	// agrego la id a lo ultimo de la cola
	queue_push(procesos_en_ready,id_create(contador_de_id_procesos));

	pthread_mutex_unlock(&ready);


	printf("Proceso %s en ejecucion....\n", path);

	sem_post(&solicitud_ejecucion);

	sleep(1);

	return 0;
}


//-------------------------------------FUNCIONES HILOS--------------------------------------------------------------------


//------------HILO ENCARGADO DE RECIBIR CONEXIONES Y MENSAJES DE CPUs----------------------------------------------------
void* recibir_cpu(){

	FD_ZERO(&master); // borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);

	int puertoConCambios=0;

	socketEscucha = crearSocket();
	asociarAPuerto(socketEscucha,puerto);

	// me pongo a escuchar conexiones
	escucharConexiones(socketEscucha,5); //se bloquea hasta q haya cpus nuevas

	FD_SET(socketEscucha, &master); // Agrega socketEscucha al master set

	if (socketEscucha > fdmax) { // es el mayor
		fdmax = socketEscucha; // guardo el mayor
	}

	recibir_conexion();

	while(1){

		read_fds = master; // backup de mi descriptores de archivo.

		// select se bloque hasta que le llegan "mensajitos".
		select(fdmax+1, &read_fds, NULL, NULL, NULL);

		for(puertoConCambios = 0; puertoConCambios <= fdmax; puertoConCambios++) {

			// preguntar a todos los puertos de "read_fds" si recibieron mensajes.
			if (FD_ISSET(puertoConCambios, &read_fds)) { //pregunta si "puertoConCambios" está en el conjunto y si hubo cambio.

				break; // encontramos el puerto donde hubo cambios, "puertoConCambios" es el puerto con cambios.
				// salgo del for.
			}
		}

		if(puertoConCambios == socketEscucha){ // pregunto si hay conexiones nuevas

			recibir_conexion();
		}else{

			recibir_rafagas();
		}


	}

}

//---------------funcion encargado de recibir conexiones de CPUs   -------------------------
int recibir_conexion(){


	int socketCpu;
	int id;


		socketCpu = crearSocketParaAceptarSolicitudes(socketEscucha); //

		recibirMensaje(socketCpu, &id, sizeof(int));// recibo id de CPU

		pthread_mutex_lock(&cpuss);

		list_add(CPUs, cpu_create(id,1,socketCpu)); // guardo en la lista

		pthread_mutex_unlock(&cpuss);

		enviarMensaje(socketCpu,&quantum,sizeof(quantum)); // le mando el quantum, que es un int

		FD_SET(socketCpu, &master); // Agrega socketCpu al master set
		if (socketCpu > fdmax) { // es el mayor
			fdmax = socketCpu; // guardo el mayor
		}

		read_fds = master; // actualizo el temporal

		sem_post(&solicitud_cpuLibre); // desperta a al hilo "ejecutar_proceso"


	return 0;
}



//-----------------------------------------------------------------------------------------------------------


//------- funcion encargado de recibir las rafagas de las CPU que vienen de: quantum/entrada_salida/fin---------
int recibir_rafagas(){


	t_CPU *nodo_cpu;
	t_PCB *PCB_recibido;
	t_PCB *PCB;
	char llegada; // "Quantum", "Bloqueado" y "Fin"

	pthread_mutex_lock(&cpuss);

	//una vez que encontramos el puerto, lo saco con "puertoConCambios" al nodo de la lista
	nodo_cpu= list_find(CPUs,(void*)(buscar_por_puerto)); // "puertoConCambios" es la variable gloval, esta en el .h

	// llegada es un protocolo de comunicacion, para saber que hacer con el PCB del proceso llegante
	recibirMensaje(nodo_cpu->puerto, &llegada, sizeof(char));// recibo llegada

	PCB_recibido = recibirPCB(nodo_cpu->puerto); // recibe el PCB


	pthread_mutex_lock(&pcbs);

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

	pthread_mutex_unlock(&pcbs);

	// agregar esa CPU como disponible();
	nodo_cpu->disponibilidad = 1;

	pthread_mutex_unlock(&cpuss);

	free(PCB_recibido);

	sem_post(&solicitud_cpuLibre); // desperta a al hilo "ejecutar_proceso"

	return 0;

}


int llega_quantum(t_PCB *PCB){

	pthread_mutex_lock(&ready);
	// meter procesos en la cola de ready
	queue_push(procesos_en_ready,id_create(PCB->id));

	pthread_mutex_unlock(&ready);

	// actualizo el PCB
	PCB->estado = 'R'; // le cambio el valor que esta en memoria dinamica

	sem_post(&solicitud_ejecucion);

	return 0;

}

int llega_entrada_salida(t_PCB *PCB){

	pthread_mutex_lock(&bloqueados);
	// meter procesos en la cola de ready
	queue_push(procesos_bloqueados,id_create(PCB->id ));

	pthread_mutex_unlock(&bloqueados);

	// actualizo el PCB
	PCB->estado = 'B'; // le cambio el valor que esta en memoria dinamica

	return 0;

}

int llega_de_fin(t_PCB *PCB){

	PCB->estado = 'F'; // le cambio el valor que esta en memoria dinamica

	return 0;

}


//-------------------------------------------------------------------------------------------------------------



//---------------HILO encargado de dormir procesesos durante un cierto tiempo "T"--------------------------
//---------------tambien los pasa a la cola de "listos" una vez pasado ese tiempo-------------------------


void* bloquear_procesos(){

	t_bloqueados *nodo_bloqueado;
	t_PCB *nodo_pcb;


	while(1){

		sem_wait(&solicitud_deBloqueo);

		pthread_mutex_lock(&bloqueados);
		// sacar un proceso de la cola de "procesos_bloqueados" (modificando la lista)
		nodo_bloqueado = queue_pop(procesos_bloqueados);

		pthread_mutex_unlock(&bloqueados);

		sleep(nodo_bloqueado->tiempo);

		pthread_mutex_lock(&pcbs);

		// una vez transcurrido ese tiempo buscar con la id en "lista_de_PCB" y sacar su PCB (sin modificar la lista)
		nodo_pcb =list_get(lista_de_PCB, nodo_bloqueado->id - 1); // se modifica la lista ?? (no se tiene q modificar!)

		// modificar su estado de: bloqueado a--> listo y meter de nuevo en "lista_de_PCB"
		nodo_pcb->estado = 'R'; // le cambio el valor que esta en memoria dinamica

		pthread_mutex_unlock(&pcbs);

		pthread_mutex_lock(&ready);
		// meter id de proceso en cola: "procesos_en_ready"
		queue_push(procesos_en_ready,id_create (nodo_bloqueado->id)); // ya hay un nuevo nodo en la cola

		pthread_mutex_unlock(&ready);

		free(nodo_bloqueado); // saco el nodo de memoria dinamica

		sem_post(&solicitud_ejecucion);

	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------

//---------------HILO encargado de mandar a ejecutar procesos a la CPUs-------------------------

void* ejecutar_proceso(){

	int *id;
	t_PCB *nodo_pcb;
	t_CPU * nodo_cpu;

	while(1){

		sem_wait(&solicitud_ejecucion);

		sem_wait(&solicitud_cpuLibre);;

		pthread_mutex_lock(&ready);
		id = queue_pop(procesos_en_ready); //sacar_primer_elemento_de_la_cola

		pthread_mutex_unlock(&ready);

		pthread_mutex_lock(&pcbs);

		nodo_pcb =list_get(lista_de_PCB, *id-1); //PCB=buscar_id_de_proceso (sin desarmar la lista)

		nodo_pcb->estado = 'E'; // le cambio el valor que esta en memoria dinamica

		pthread_mutex_lock(&cpuss);

		//buscar_CPU_disponible. Esta funcion me devuelve un puerto libre
		nodo_cpu=list_find(CPUs,(void*)diponibilidad); // siempre tiene q haber un puerto libre, o explota el programa

		// mandar_PCB_al_cpu();

		pthread_mutex_unlock(&pcbs);

		// seteo esa cpu como ocupada
		nodo_cpu->puerto = 0;

		pthread_mutex_unlock(&cpuss);

		free(id);


	}
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
			printf("#     5) Salir                                                 #\n");
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
			case 5:
			   return 0;	    break;

			default: printf("Opción incorrecta. Por favor ingrese una opción del 1 al 4 \n"); break;
		  }

	}

 return 0;

}

void crear_lista(){

	lista_de_PCB = list_create(); //Crea la lista_de_PCB
	procesos_en_ready = queue_create(); //Crea la cola de pocesos en ready
	CPUs = list_create(); // crea lista de CPUs conectadas
	procesos_bloqueados = queue_create(); // crea cola de procesos bloqueados
}

void inicializar_semaforos(){

	//Declaración de Mutex.
		pthread_mutex_init(&pcbs,NULL);
		pthread_mutex_init(&cpuss,NULL);
		pthread_mutex_init(&ready,NULL);
		pthread_mutex_init(&bloqueados,NULL);

		// Inicialización de Semáforos en 0.
		sem_init(&solicitud_ejecucion, 1, 0);
		sem_init(&solicitud_cpuLibre, 1, 0);
		sem_init(&solicitud_deBloqueo, 1, 0);

}

void liberar_memoria(){

	liberarSocket(socketEscucha);

	//destruir hilos
	//destruir listas.todo lo q este en memoria dinamica.
	pthread_mutex_destroy(&pcbs);
	pthread_mutex_destroy(&cpuss);
	pthread_mutex_destroy(&ready);
	pthread_mutex_destroy(&bloqueados);


	sem_destroy(&solicitud_ejecucion);
	sem_destroy(&solicitud_cpuLibre);
	sem_destroy(&solicitud_deBloqueo);

}
