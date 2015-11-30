#include <stdio.h>
#include <stdlib.h>
#include "funcionesMemoria.h"
#include <commonsDeAsedio/cliente-servidor.h>
#include <commons/collections/list.h>
#include <commonsDeAsedio/select.h>
#include <commonsDeAsedio/thread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "impresionesEnPantalla.h"
#include "funcionesSeniales.h"
#include <signal.h>
#include <sys/time.h>
//---------------------------------------------------------------
#define maxConexionesEntrantes 10

#define ESPERA_EN_MICROSEGUNDOS 0//Esto lo agrego para
#define ESPERA_EN_SEGUNDOS 1//que select no sea bloqueante

//SEÑALES
void crearHijoYPadre(){

	if((idHijo = fork()) == 0){
		//hijo
		volcarRamALog();
		exit(0);
	}else {
		//padre
		wait(&estado);
	}
}

int main(void) {

//////////////////////////INICIALIZACION DE VARIABLES////////////////////////////////

	tipoConfigMemoria* configuracion = cargarArchivoDeConfiguracionDeMemoria("cfgMemoria");

	int socketParaCpus = crearSocket();

	int socketParaSwap = crearSocket();

	int socketCpuEntrante;

	bool memoriaActiva = true;

	struct timeval tiempoDeEsperaDeCpus;

	tiempoDeEsperaDeCpus.tv_sec = ESPERA_EN_SEGUNDOS;

	tiempoDeEsperaDeCpus.tv_usec = ESPERA_EN_MICROSEGUNDOS;
//--------------ACA EMPIEZA FERNILANDIA--------------------------

	fd_set listaPrincipal;
	fd_set listaFiltrada;

	FD_ZERO(&listaPrincipal);
	FD_ZERO(&listaFiltrada);
	FD_SET(socketParaCpus,&listaPrincipal);

	datosMemoria = malloc(sizeof(tipoEstructuraMemoria));

	datosMemoria->socketSWAP = socketParaSwap;

	datosMemoria->maximoSocket = socketParaCpus;

	datosMemoria->configuracion = configuracion;

	datosMemoria->memoriaActiva = &memoriaActiva;

	pthread_mutex_t mutex;

	inicializarMutex(&mutex);

	datosMemoria->socketCpus = socketParaCpus;

	listaSeniales = list_create();//Lista de señales

	/*system("if [ -f logDeTLB ]; then rm logDeTLB\nfi");
	system("if [ -f logDeSwapeo ]; then rm logDeSwapeo\nfi");
	system("if [ -f logDeSeniales ]; then rm logDeSeniales\nfi");
	system("if [ -f logDeAlgoritmos ]; then rm logDeAlgoritmos\nfi");
	system("if [ -f logMemoria ]; then rm logMemoria\nfi");//Si ya existe el log lo borra
*/
	datosMemoria->logDeMemoria = log_create("logDeMemoria","Administrador de Memoria",false,LOG_LEVEL_TRACE);
	datosMemoria->logDeAlgoritmos = log_create("logDeAlgoritmos","Administrador de Memoria",false,LOG_LEVEL_TRACE);
	datosMemoria->logDeTLB = log_create("logDeTLB","Administrador de Memoria",false,LOG_LEVEL_TRACE);
	datosMemoria->logDeSwapeo = log_create("logDeSwapeo","Administrador de Memoria",false,LOG_LEVEL_TRACE);
	datosMemoria->logDeSeniales = log_create("logDeSeniales","Administrador de Memoria",false,LOG_LEVEL_TRACE);

	setearEstructuraMemoria(datosMemoria);

//-------------END OF FERNILANDIA-----------------------------------

	asociarAPuerto(socketParaCpus,configuracion->puertoDeEscucha);

	conectarAServidor(socketParaSwap,configuracion->ipSWAP,configuracion->puertoSWAP);

	escucharConexiones(socketParaCpus,maxConexionesEntrantes);

	pthread_t hiloTasaTLB,hiloMenu,hiloSeniales;

	crearThread(&hiloTasaTLB,mostrarTasaTLBPeriodicamente,NULL);

	crearThread(&hiloMenu,funcionPrueba,datosMemoria);

	//SEÑALES
	crearThread(&hiloSeniales,crearSeniales,NULL);
/////////////////////////////////////////////////////////////////////////////////////

	pantallaDeInicio();

	while(memoriaActiva){

		//SEÑALES no esoty usando el hilo
		/*signal(SIGUSR1, prepararSenialLimpiarTLB);
		signal(SIGUSR2, prepararSenialLimpiarRAM);
		signal(SIGPOLL, prepararSenialVolcarRamALog);*/

		listaFiltrada = listaPrincipal;
		select(datosMemoria->maximoSocket+1,&listaFiltrada,NULL,NULL,&tiempoDeEsperaDeCpus);//NULL);

		int var;
		for (var = 0; var <= datosMemoria->maximoSocket; var++) {

			//SEÑALES
			tratarSenial();

			if(FD_ISSET(var, &listaFiltrada)){

				if(var==datosMemoria->socketCpus){
					socketCpuEntrante = crearSocketParaAceptarSolicitudes(datosMemoria->socketCpus);
					FD_SET(socketCpuEntrante,&listaPrincipal);
					datosMemoria->maximoSocket = maximoEntre(datosMemoria->maximoSocket,socketCpuEntrante);
				}
				else tratarPeticion(var);
			}
		}

	}

	terminarLogs();

	free(datosMemoria);
	liberarSocket(socketParaCpus);
	liberarSocket(socketParaSwap);
	destruirConfigMemoria(configuracion);

	printf("Memoria finalizada");

	return EXIT_SUCCESS;
}
