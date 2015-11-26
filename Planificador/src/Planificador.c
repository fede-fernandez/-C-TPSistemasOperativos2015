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
#include <commons/log.h>


//---->>>>>>>VARIABLES GLOBALES <<<<<<<<<<<<------------

int contador_de_id_procesos = 0; // para saber cuantos procesos hay en el sistema
int quantum = 0; // 0 si tiene quantum y el "valor" en caso de que tenga quantum


//---listas y colas

t_list *lista_de_PCB;
t_queue *procesos_en_ready; // cola de procesos "listos para ejecutar"
t_list *CPUs; // lista de cpu disponibles
t_queue *procesos_bloqueados; // cola donde se encolan los procesos para luego mandarlos a dormir
t_list *id_finalizados; // mProc finalizados forsozamente

//-- sockets
int puerto = 4000;//Elijo 7200 pero esto se carga del archivo de configuracion
int socketMaestro; // socket Maeestro se conecta al principio
int socketEscucha; // socket que escuche las conecciones entrantes
fd_set master; // conjunto maestro de descriptores de fichero
fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
int fdmax = 0; // número máximo de descriptores de fichero

int puertoConCambios; // puerto donde hubo cambios

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


// ------log

t_log* proceso; //"comienzo y fin de mproc"
t_log* conexiones; // conexiones a CPUs
t_log* rafagas; // ragagas de CPUs completadas
t_log* colita; // colas de ready
t_log* metricas;

// ---- Mutex log

// falta un mutex ?
pthread_mutex_t mutex_log2;
pthread_mutex_t mutex_log3;

void conectar_maestroCPU(){

	socketEscucha = crearSocket();

	asociarAPuerto(socketEscucha,puerto);

	printf("\n\n    ********* Esperando a que cpu se conecte.. \n\n");

	// me pongo a escuchar conexiones
	escucharConexiones(socketEscucha,5); //se bloquea hasta q haya cpus nuevas

	socketMaestro = crearSocketParaAceptarSolicitudes(socketEscucha); //


}


int main(void) {

	configurar();

	conectar_maestroCPU();

	logueo();

	inicializar_semaforos();

	crear_lista();

	//Este hilo va a escuchar y aceptar las conexiones, con las CPU de forma paralela a la ejecucion de este proceso "main"
	pthread_create(&ejecucion, NULL, ejecutar_proceso, NULL);
	pthread_create(&recibir, NULL, recibir_cpu, NULL);
	pthread_create(&bloquear, NULL, bloquear_procesos, NULL);

	menu();

	liberar_memoria();

	printf("fin =) ");
	return 0;
}

int correr_path(void){



	char path[30];


  //limpiar pantalla
	//system("clear");

	printf("Ingresar Comando: \n");

	scanf("%s", path); // supongo que siempre es un comando valido y path tambien

	contador_de_id_procesos++; // mantengo la cuenta de todos los procesos que se crearon en el sistema

	pthread_mutex_lock(&pcbs);
	// Agrego el elemento al final de la lista (para que quede ordenada por ID)
	list_add(lista_de_PCB, PCB_create(contador_de_id_procesos, 1, 'R', path, time(NULL), time(NULL)));

	pthread_mutex_unlock(&pcbs);

	pthread_mutex_lock(&ready);
	// agrego la id a lo ultimo de la cola
	queue_push(procesos_en_ready,id_create(contador_de_id_procesos));

	pthread_mutex_unlock(&ready);

	printf("Proceso %s en ejecucion....\n", path);

	log_trace(proceso, "              INICIO --> ID_mproc: %d , nombre: %s ",contador_de_id_procesos, path);

	sem_post(&solicitud_ejecucion);


	//sleep(1);

	return 0;
}


//-------------------------------------FUNCIONES HILOS--------------------------------------------------------------------


//------------HILO ENCARGADO DE RECIBIR CONEXIONES Y MENSAJES DE CPUs----------------------------------------------------
void* recibir_cpu(){

	FD_ZERO(&master); // borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);

	FD_SET(socketEscucha, &master); // Agrega socketEscucha al master set

	if (socketEscucha > fdmax) { // es el mayor
		fdmax = socketEscucha; // guardo el mayor
	}

	recibir_conexion();

	while(1){

		puertoConCambios=0;

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

		recibirMensajeCompleto(socketCpu, &id, sizeof(int));// recibo id de CPU


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
	t_PCB PCB_recibido;
	t_PCB *PCB;
	char llegada; // "Quantum", "Bloqueado" y "Fin"
	int T; // tiempo que se va a dormir el mcod


	pthread_mutex_lock(&cpuss);

	//una vez que encontramos el puerto, lo saco con "puertoConCambios" al nodo de la lista
	nodo_cpu= list_find(CPUs,(void*)(buscar_por_puerto)); // "puertoConCambios" es la variable gloval, esta en el .h

	// llegada es un protocolo de comunicacion, para saber que hacer con el PCB del proceso llegante
	recibirMensajeCompleto(nodo_cpu->puerto, &llegada, sizeof(char));// recibo llegada


	if(llegada=='B'){

		recibirMensajeCompleto(nodo_cpu->puerto, &T, sizeof(int));
	}

	PCB_recibido = recibirPCB2(nodo_cpu->puerto); // recibe el PCB

	log_rafagas(nodo_cpu->puerto); // logueo el resultado de las rafagas


	pthread_mutex_lock(&pcbs);

	// buscar id de proceso en "lista_de_PCB"
	PCB = buscar_PCB(lista_de_PCB, PCB_recibido.id);


	//cuento el tiempo q estuvo ejecutandose
	PCB->tiempoEjecucion = PCB->tiempoEjecucion + (difftime(time(NULL),PCB->ultimaEjecucion));


	if( !(estas_finalizado(id_finalizados,PCB->id))){ // verifico que ese id NO este finalizado Forsazamente

		PCB->pc = PCB_recibido.pc; // actualizo el contador de programa PCB

	}


	switch (llegada) {
	  case   'Q':
	      llega_quantum(PCB);	       break; // va a meter ese proceso a la cola de redy y actualizar PCB
	  case   'B':
		  llega_entrada_salida(PCB,T);   break; // va a meter ese proceso a la cola de Entara-Salida, para despues bloquearlo y actualizar PCB
	  case   'F':
	      llega_de_fin(PCB);	       break; // unicamente actualiza el PCB del proceso llegante

	}

	log_actividad_cpus(nodo_cpu->id_cpu,"DISPONIBLE", PCB->id, PCB->path, "recibido");

	pthread_mutex_unlock(&pcbs);

	// agregar esa CPU como disponible();
	nodo_cpu->disponibilidad = 1;

	pthread_mutex_unlock(&cpuss);

	sem_post(&solicitud_cpuLibre); // desperta a al hilo "ejecutar_proceso"

	return 0;

}


int llega_quantum(t_PCB *PCB){

	pthread_mutex_lock(&ready);
	// meter procesos en la cola de ready
	queue_push(procesos_en_ready,id_create(PCB->id));

	PCB->ultimaEspera = time(NULL); // guardo el tiempo en que entro a la cola de ready

	pthread_mutex_unlock(&ready);

	// actualizo el PCB
	PCB->estado = 'R'; // le cambio el valor que esta en memoria dinamica

	log_colas(PCB->id,PCB->path,"volvio por quantum de:",quantum);

	sem_post(&solicitud_ejecucion);

	return 0;

}

int llega_entrada_salida(t_PCB *PCB,int T){


	// meter procesos en la cola de bloqueados
	pthread_mutex_lock(&bloqueados);
	queue_push(procesos_bloqueados,bloquedito_create(PCB->id,T));
	pthread_mutex_unlock(&bloqueados);

	// actualizo el PCB
	PCB->estado = 'B'; // le cambio el valor que esta en memoria dinamica

	sem_post(&solicitud_deBloqueo);

	return 0;

}

int llega_de_fin(t_PCB *PCB){

	double respuesta;

	PCB->estado = 'F'; // le cambio el valor que esta en memoria dinamica

	log_trace(proceso, "              FIN --> ID_mProc: %d , mcod: %s ",PCB->id, PCB->path);

	respuesta = difftime(time(NULL),PCB->ultimaRespuesta);

	log_info(metricas, "        ID_mProc: %d -> %s | tiempo de respuesta: %f seg | tiempo de ejecucion: %f seg | tiempo de espera: %f seg \n", PCB->id, PCB->path , respuesta,PCB->tiempoEjecucion, PCB->tiempoEspera);

	liberar_pcb(lista_de_PCB, PCB);

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

		// una vez transcurrido ese tiempo buscar con la id en "lista_de_PCB" y sacar su PCB
		nodo_pcb = buscar_PCB(lista_de_PCB, nodo_bloqueado->id);

		// modificar su estado de: bloqueado a--> listo y meter de nuevo en "lista_de_PCB"
		nodo_pcb->estado = 'R'; // le cambio el valor que esta en memoria dinamica

		log_colas(nodo_pcb->id,nodo_pcb->path,"volvio por una E/S de:",nodo_bloqueado->tiempo);

		nodo_pcb->ultimaEspera = time(NULL); // guardo el tiempo en que entro a la cola de ready

		pthread_mutex_unlock(&pcbs);

		pthread_mutex_lock(&ready);
		// meter id de proceso en cola: "procesos_en_ready"
		queue_push(procesos_en_ready,id_create (nodo_bloqueado->id)); // ya hay un nuevo nodo en la cola

		pthread_mutex_unlock(&ready);

		free(nodo_bloqueado);

		sem_post(&solicitud_ejecucion);

	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------

//---------------HILO encargado de mandar a ejecutar procesos a la CPUs-------------------------

void* ejecutar_proceso(){

	int *id;
	t_PCB *nodo_pcb;
	t_PCB pcb;
	t_CPU * nodo_cpu;

	while(1){


		sem_wait(&solicitud_ejecucion);

		sem_wait(&solicitud_cpuLibre);

		pthread_mutex_lock(&ready);
		id = queue_pop(procesos_en_ready); //sacar_primer_elemento_de_la_cola

		pthread_mutex_unlock(&ready);

		pthread_mutex_lock(&pcbs);

		nodo_pcb = buscar_PCB(lista_de_PCB, *id); //PCB=buscar_id_de_proceso (sin desarmar la lista)

		nodo_pcb->estado = 'E'; // le cambio el valor que esta en memoria dinamica

		pcb = *nodo_pcb;

		nodo_pcb->ultimaEjecucion = time(NULL); // grabo los segundos del sistema

		//guardo el tiempo final desde que entro a ready hasta que salio
		nodo_pcb->tiempoEspera = nodo_pcb->tiempoEspera + (difftime(time(NULL),nodo_pcb->ultimaEspera));

		pthread_mutex_unlock(&pcbs);

		pthread_mutex_lock(&cpuss);

		//buscar_CPU_disponible. Esta funcion me devuelve un puerto libre
		nodo_cpu=list_find(CPUs,(void*)diponibilidad); // siempre tiene q haber un puerto libre, o explota el programa


		//enviarPCB(nodo_cpu->puerto,pcb);
		enviarPCB2(nodo_cpu->puerto,pcb);

		// seteo esa cpu como ocupada
		nodo_cpu->disponibilidad = 0;

		log_actividad_cpus(nodo_cpu->id_cpu,"TRABAJANDO..", pcb.id, pcb.path,"ejecutando" );

		pthread_mutex_unlock(&cpuss);

		free(id);


	}
}

//--------------------------------------------------------------------------------------------------------

int menu(void) {

	int opcion;
	char opchar[2];
	char fin= 'S'; // mando una 'S' para q todos los procesos terminen


	while(1) // el menu tiene que estar presente siempre

	{
		 // limpiar pantalla

		memset(opchar, '\0', 2);

		opcion = 0;


		    printf("############################################# L ##################\n");
			printf("################################################################\n");
			printf("##     --------> *****                  ***** <------------   ##\n");
			printf("##   *****             LOS  JAVIMANCOS         ***** -------  ##\n");
			printf("##------------------------------------------------------------##\n");
			printf("##                                                            ##\n");
			printf("##      Ingrese una opción para continuar:                    ##\n");
			printf("##                                                            ##\n");
			printf("##                                                            ##\n");
			printf("##          1) Correr Path                                    ##\n");
			printf("##                                                            ##\n");
			printf("##          2) Finalizar PID                                  ##\n");
			printf("##                                                            ##\n");
			printf("##          3) ps                                             ##\n");
			printf("##                                                            ##\n");
			printf("##          4) cpu                                            ##\n");
			printf("##                                                            ##\n");
			printf("##          5) Salir                                          ##\n");
			printf("##                                                            ##\n");
			printf("################################################################\n");
			printf("################################################################\n\n\n");

			scanf("%s", opchar);

			opcion = atoi(opchar);

		 switch (opcion) {
			case 1:

			   correr_path();	break; // las demas funciones las puedo desarrollar a lo ultimo

			case 2:

			   finalizar_PID(); break;

			case 3:

			   ps();	        break;

			case 4:

			   cpu();	        break;

			case 5:

				enviarMensaje(socketMaestro, &fin, sizeof(char));

				tipoRespuesta* respuesta = recibirRespuesta(socketMaestro);

				if(respuesta->respuesta==PERFECTO){
				destruirTipoRespuesta(respuesta);
					return 0;	
				}


				break;

			default: printf("Opción incorrecta. Por favor ingrese una opción del 1 al 4 \n \n \n"); break;
		  }

	}

 return 0;

}

void finalizar_PID(){

	char mensajito = 'C';
	int ultima; // cantidad de instrucciones
	int id;

	t_PCB *nodo_pcb;

	printf("\n   Ingrese el ID_mProc que desea finalizar: \n\n");

	scanf("%d",&id);

	enviarMensaje(socketMaestro,&mensajito,sizeof(char)); // le mando el quantum, que es un int


	pthread_mutex_lock(&pcbs);

	nodo_pcb = buscar_PCB(lista_de_PCB, id); //PCB=buscar_id_de_proceso (sin desarmar la lista)


	enviarPath(socketMaestro ,nodo_pcb); // envio el path al CPU


	pthread_mutex_unlock(&pcbs);

	recibirMensajeCompleto(socketMaestro, &ultima, sizeof(int));// recibo la ultima instruccion

	pthread_mutex_lock(&pcbs);

	nodo_pcb->pc = ultima; // le cambio el estado a finalizado

	list_add(id_finalizados,id_create(nodo_pcb->id));

	pthread_mutex_unlock(&pcbs);


}

void ps(){

	int i=0;
	t_PCB *PCB;
	int tamano;
	char timer[10];

	pthread_mutex_lock(&pcbs);

	tamano = list_size(lista_de_PCB);


	for(i = 0; i < tamano; i++) {

		PCB =list_get(lista_de_PCB, i);
		printf("mProc %d: %s --> %c \n", PCB->id,PCB->path,PCB->estado);
	}

	pthread_mutex_unlock(&pcbs);

	printf("\n   Ingrese un caracter y presione enter para volver al menu \n\n");

	scanf("%s",timer);
}



// porcenjes de usos de CPUs
void cpu(){

	int i=0;
	int tamano;
	int porcentaje;
	char timer[10];
	int contador = 1;
	char mensajito = '%';


	enviarMensaje(socketMaestro,&mensajito,sizeof(char)); // le mando el %, que es un int


	pthread_mutex_lock(&cpuss);

	tamano = list_size(CPUs);

	pthread_mutex_unlock(&cpuss);

	for(i = 0; i < tamano; i++){

		recibirMensajeCompleto(socketMaestro, &porcentaje, sizeof(int));// recibo los porcentajes

		printf("ID_CPU: %d --> %d %% \n", contador, porcentaje);

		contador++;

	}



	printf("\n   Ingrese un caracter y presione enter para volver al menu \n\n");


	scanf("%s",timer);


}





void liberar_PCBs(){

	int i=0;
	t_PCB *PCB;
	int tamano;


	pthread_mutex_lock(&pcbs);

	tamano = list_size(lista_de_PCB);


	for(i = 0; i < tamano; i++) {

		PCB =list_get(lista_de_PCB, i);

		free(PCB);

	}

	pthread_mutex_unlock(&pcbs);


}

int buscar_por_puerto(t_CPU *nodo){

	if(nodo->puerto == puertoConCambios){

		return 1;
	}

	return 0;
}






//----------------------Iniciallizo y libero memoria

void crear_lista(){

	lista_de_PCB = list_create(); //Crea la lista_de_PCB
	procesos_en_ready = queue_create(); //Crea la cola de pocesos en ready
	CPUs = list_create(); // crea lista de CPUs conectadas
	procesos_bloqueados = queue_create(); // crea cola de procesos bloqueados
	id_finalizados = list_create();
}


void inicializar_semaforos(){

	//Declaración de Mutex.
		pthread_mutex_init(&pcbs,NULL);
		pthread_mutex_init(&cpuss,NULL);
		pthread_mutex_init(&ready,NULL);
		pthread_mutex_init(&bloqueados,NULL);
		pthread_mutex_init(&mutex_log2,NULL);
		pthread_mutex_init(&mutex_log3,NULL);


		// Inicialización de Semáforos en 0.
		sem_init(&solicitud_ejecucion, 1, 0);
		sem_init(&solicitud_cpuLibre, 1, 0);
		sem_init(&solicitud_deBloqueo, 1, 0);

}

int liberar_memoria(){


	liberarSocket(socketEscucha);




	//list_destroy_and_destroy_elements(CPUs,(void*)liberar_pcb);s

	//destruir hilos
	//destruir listas.todo lo q este en memoria dinamica.
	pthread_mutex_destroy(&pcbs);
	pthread_mutex_destroy(&cpuss);
	pthread_mutex_destroy(&ready);
	pthread_mutex_destroy(&bloqueados);


	sem_destroy(&solicitud_ejecucion);
	sem_destroy(&solicitud_cpuLibre);
	sem_destroy(&solicitud_deBloqueo);

return 0;

}

void liberar_puertos(){

	t_CPU* nodo_cpu;
	int tamano;
	int i;

	tamano = list_size(CPUs);

	for(i = 0; i < tamano; i++) {

		nodo_cpu =list_get(CPUs, i);
		liberarSocket(nodo_cpu->puerto);
		free(nodo_cpu);
	}

}


void configurar(){

	tipoConfig *configuracion;

	//Carga de archivo de configuracion
	configuracion = cargarArchivoDeConfiguracion("cfgPlanificador");

	puerto = configuracion->puertoPlanificador;
	quantum = configuracion->quantum;

	printf("Quantum: %d \n", quantum);

	destruirConfig(configuracion);

}

void logueo(){

	colita = log_create("cola de ready", "PLANIFICADOR", 0, LOG_LEVEL_TRACE);

	proceso = log_create("comienzo y fin de mproc", "PLANIFICADOR", 0, LOG_LEVEL_TRACE);

	conexiones = log_create("actividad CPUs", "PLANIFICADOR", 0, LOG_LEVEL_TRACE);

	rafagas = log_create("resultado de Ráfagas", "PLANIFICADOR", 0, LOG_LEVEL_INFO);

	metricas = log_create(" metricas ", "PLANIFICADOR", 0, LOG_LEVEL_INFO);


}

void log_actividad_cpus(int id_cpu, char estado[15],int id_proceso,char path[30], char estado_pcb[15]){

	pthread_mutex_lock(&mutex_log2);

	log_trace(conexiones, "              cpu_ID: %d -->  %s | ID_mProc: %d --> %s ( %s ) ",id_cpu, estado,id_proceso,path,estado_pcb);

	pthread_mutex_unlock(&mutex_log2);
}

void log_colas(int id_proceso,char path[30],char razon[40],int numero){

	pthread_mutex_lock(&mutex_log3);

	log_trace(colita, "              ID_mProc: %d -->  %s | ingresa a la cola de ready, porque %s %d ",id_proceso, path,razon, numero);

	pthread_mutex_unlock(&mutex_log3);

}




void log_rafagas(int sockCpu){
	size_t tamanioDeResultadoDeRafaga;
	char* resultadoDeRafaga;
	recibirMensajeCompleto(sockCpu, &tamanioDeResultadoDeRafaga, sizeof(size_t));
	resultadoDeRafaga = malloc(tamanioDeResultadoDeRafaga);
	recibirMensajeCompleto(sockCpu, resultadoDeRafaga, tamanioDeResultadoDeRafaga);



	log_info(rafagas, "\n %s \n", resultadoDeRafaga);

	free(resultadoDeRafaga);

}
