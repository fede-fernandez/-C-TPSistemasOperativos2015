#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include "funcionesPlanificador.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <commonsDeAsedio/cliente-servidor.h>

//--------------- -------estructuras, esto va en el .h------------------------------------------

//Las movi al header pero dejo los comentarios momentaneamente

// -------------------------------------------------------------------------------------------------------


//-------------------------------------FUNCIONES HILOS--------------------------------------------------------------------

//---------------Hilo encargado de mandar a ejecutar procesos a la CPUs-------------------------

void* ejecutar_proceso(void){

	/*while(1){

		// wait(cant_procesos_listos);
		// wait(cant_CPUs_libres);
		//sacar_ultimo_elemento_de_la_lista( procesos_en_ready, id );
		//PCB=buscar_id_de_proceso(lista_de_PCB , id);
		//actualizar_PCB(PCB) ----[de listo a--> ejecucion]
		//buscar_CPU_disponible(cpu_disponibles , cpu_puerto)
		//send(cpu_puerto, PCB, strlen(PCB) + 1, 0);

	}*///Lo comento momentaneamente para hacer el checkpoint 1

	int puerto = 7200;//Elijo 7200 pero esto se carga del archivo de configuracion

	int socket = crearSocket();

	asociarAPuerto(socket,puerto);

	escucharConexiones(socket,1);

	int socketCpu = crearSocketParaAceptarSolicitudes(socket);

	char mensaje[30] = "correr programa";

	enviarMensaje(socket,&mensaje,sizeof(mensaje));

	printf("Enviado comando 'correr programa'\n");

	liberarSocket(socketCpu);

	liberarSocket(socket);

	getchar();

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
				      quantum(PCB);	         break; // va a meter ese proceso a la cola de redy y actualizar PCB
				  case   'B':
				      entrada_salida(PCB);   break; // va a meter ese proceso a la cola de Entara-Salida, para despues bloquearlo y actualizar PCB
				  case   'F':
				      fin(PCB);	             break; // unicamente actualiza el PCB del proceso llegante
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
//---------------Hilo encargado de dormir procesesos durante un cierto tiempo "T"--------------------------
//---------------tambien los pasa a la cola de "listos" una vez pasado ese tiempo-------------------------


void* bloquear_procesos(void){

	while(1){

		// sacar un proceso de la cola de "procesos_bloqueados" (modificando la lista)
		// dormirlo segun indique su campo "tiempo"
		// una vez transcurrido ese tiempo buscar con la id en "lista_de_PCB" y sacar su PCB (sin modificar la lista)
		// modificar su estado de: bloqueado a--> listo y meter de nuevo en "lista_de_PCB"
		// meter id de proceso en cola: "procesos_en_ready"

	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------




// Variables Globales

int contador_de_id_procesos = 0; // para saber cuantos procesos hay en el sistema
t_list* lista_de_PCB;
t_list* procesos_en_ready; // lista de procesos "listos para ejecutar"
t_list* cpu_disponibles; // lista de cpu disponibles
t_list* procesos_bloqueados; // lista donde se encolan los procesos para luego mandarlos a dormir
int socketEscucha; // socket que escuche las conecciones entrantes

int correr_path(void);

int menu(void) {

	while(1) // el menu tiene que estar presente siempre

	{
		 // limpiar pantalla

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
	pthread_t bloquear; // hilo que manda a dormir procesos que estan en la lista de "procesos_bloqueados"

	//Este hilo va a escuchar y aceptar las conexiones, con las CPU de forma paralela a la ejecucion de este proceso "main"
	pthread_create(&escucha, NULL, recibir_conexion, NULL); // falta implementar la funcion "recibir_conexion"
	pthread_create(&ejecucion, NULL, ejecutar_proceso, NULL); // falta implementar la funcion "ejecutar_proceso"
	pthread_create(&recibir, NULL, recibir_rafagas, NULL); // falta implementar la funcion
	pthread_create(&bloquear, NULL, bloquear_procesos, NULL); //falta implementar la funcion


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
