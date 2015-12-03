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
#include "funcionPrincipal.h"
//---------------------------------------------------------------
#define maxConexionesEntrantes 10

//#define ESPERA_EN_MICROSEGUNDOS 0//Esto lo agrego para
//#define ESPERA_EN_SEGUNDOS 1//que select no sea bloqueante

//SEÑALES
extern pthread_mutex_t mutexTurnoSenial;
extern pthread_mutex_t mutexLimpiarTLB;
extern pthread_mutex_t mutexLimpiarRam;
extern pthread_mutex_t mutexDump;

void crearHijoYPadre(){
	idHijo = fork();

	if(idHijo == 0){
		volcarRamALog();
		exit(0);
	}
	else wait(&estado);
}

//struct timeval tiempoDeEsperaDeCpus;
fd_set listaPrincipal;
fd_set listaFiltrada;

int main(void) {

//////////////////////////INICIALIZACION DE VARIABLES////////////////////////////////

	tipoConfigMemoria* configuracion = cargarArchivoDeConfiguracionDeMemoria("cfgMemoria");

	int socketParaCpus = crearSocket();

	int socketParaSwap = crearSocket();

	bool memoriaActiva = true;

	//tiempoDeEsperaDeCpus.tv_sec = ESPERA_EN_SEGUNDOS;

	//tiempoDeEsperaDeCpus.tv_usec = ESPERA_EN_MICROSEGUNDOS;
//--------------ACA EMPIEZA FERNILANDIA--------------------------

	FD_ZERO(&listaPrincipal);
	FD_ZERO(&listaFiltrada);
	FD_SET(socketParaCpus,&listaPrincipal);

	datosMemoria = malloc(sizeof(tipoEstructuraMemoria));
		datosMemoria->socketSWAP = socketParaSwap;
		datosMemoria->maximoSocket = socketParaCpus;
		datosMemoria->configuracion = configuracion;
		datosMemoria->memoriaActiva = &memoriaActiva;

	datosMemoria->socketCpus = socketParaCpus;

	//SEÑALES
	//staSeniales = list_create();

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

	pthread_t hiloTasaTLB,hiloMenu;

	crearThread(&hiloTasaTLB,mostrarTasaTLBPeriodicamente,NULL);
	crearThread(&hiloMenu,funcionPrueba,datosMemoria);

	//SEÑALES
	inicializarMutex(&mutexTurnoSenial);

	inicializarMutex(&mutexLimpiarTLB);
		bloquearRecurso(&mutexLimpiarTLB);
	inicializarMutex(&mutexLimpiarRam);
		bloquearRecurso(&mutexLimpiarRam);
	inicializarMutex(&mutexDump);
		bloquearRecurso(&mutexDump);

	pthread_t hiloTLB, hiloRAM, hiloDump;
		crearThread(&hiloTLB,prepararSenialLimpiarTLB,NULL);
		crearThread(&hiloRAM,prepararSenialLimpiarRAM,NULL);
		crearThread(&hiloDump,prepararSenialVolcarRamALog,NULL);

		pthread_t hiloPrincipal;

		crearThread(&hiloPrincipal,funcionPrincipal,NULL);

	signal(SIGUSR1, tratarSenial);
	signal(SIGUSR2, tratarSenial);
	signal(SIGPOLL, tratarSenial);
/////////////////////////////////////////////////////////////////////////////////////

	pantallaDeInicio();

	while(memoriaActiva){

	}

	terminarLogs();

	free(datosMemoria);
	liberarSocket(socketParaCpus);
	liberarSocket(socketParaSwap);
	destruirConfigMemoria(configuracion);

	//SEÑALES
	destruirMutex(&mutexDump);
	destruirMutex(&mutexLimpiarRam);
	destruirMutex(&mutexLimpiarTLB);
	destruirMutex(&mutexTurnoSenial);

	printf(BLANCO"Proceso Administrador de Memoria "ROJO"finalizado"BLANCO".\n\n"FINDETEXTO);

	return EXIT_SUCCESS;
}
