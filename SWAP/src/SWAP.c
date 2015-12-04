#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>
#include <commonsDeAsedio/log.h>

#include "estructurasSWAP.h"
#include "funcionesSWAP.h"
#include "huecosSWAP.h"
#include "particionSWAP.h"
#include "instruccionesSWAP.h"


#define MAXIMAS_CONEXIONES_ENTRANTES 1

int main(void) {

	printf(BLANCO "Proceso SWAP " VERDE "Iniciado" BLANCO ".\n" FINDETEXTO);

	t_log* logger = crearLoggerParaSeguimiento("logSWAP","Administrador de SWAP");
	logearSeguimiento("Proceso iniciado",logger);

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("cfgSWAP");
	t_list* listaDeHuecosUtilizados = inicializarListaDeHuecosUtilizados();
	t_list* listaDeHuecosLibres = inicializarListaDeHuecosLibres(configuracion->cantidadDePaginas);
	inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);

	tipoInstruccion* instruccionAEjecutar;
	tipoRespuesta* respuestaParaMemoria;

	bool finalizarProceso = false;



/*************Sockets***************/
	int socketParaAdministrador = crearSocket();
	asociarAPuerto(socketParaAdministrador,configuracion->puertoDeEscucha);

//	printf("Escuchando conexiones...\n");
	escucharConexiones(socketParaAdministrador,MAXIMAS_CONEXIONES_ENTRANTES);
	int socketParaRecibirInstrucciones = crearSocketParaAceptarSolicitudes(socketParaAdministrador);
//	printf("Conexion establecida\n\n");


	//int i = 1;
/***********Recibir instrucciones y ejecutarlas***********/
	while(!finalizarProceso){

		//printf("Esperando instruccion\n");
		instruccionAEjecutar = recibirInstruccion(socketParaRecibirInstrucciones);
		//printf("Instruccion recibida\n");


		respuestaParaMemoria = ejecutarInstruccion(instruccionAEjecutar,listaDeHuecosUtilizados,listaDeHuecosLibres,configuracion,logger, &finalizarProceso);

		enviarRespuesta(socketParaRecibirInstrucciones,respuestaParaMemoria);
		//printf("Respuesta enviada\n\n");

		//printf("Lista de huecos utilizados:\n");
		//imprimirListaDeHuecosUtilizados(listaDeHuecosUtilizados);

	//	printf("Lista de huecos libres:\n");
		//imprimirListaDeHuecosLibres(listaDeHuecosLibres);

		destruirTipoRespuesta(respuestaParaMemoria);
		destruirTipoInstruccion(instruccionAEjecutar);

	//	i++;
	}




/********Liberar memoria*************/


//	printf("Libero sockets\n");
	liberarSocket(socketParaRecibirInstrucciones);
	liberarSocket(socketParaAdministrador);

//	printf("Libero estructuras\n");
	list_destroy_and_destroy_elements(listaDeHuecosUtilizados,(void*)destruirHuecoUtilizado);
	list_destroy_and_destroy_elements(listaDeHuecosLibres,(void*)destruirHuecoLibre);
	destruirConfigSWAP(configuracion);
	
	logearSeguimiento("Proceso finalizado",logger);
	
	destruirLogger(logger);

		printf(BLANCO "\nProceso SWAP " ROJO "Finalizado" BLANCO ".\n" FINDETEXTO);


	return EXIT_SUCCESS;
}

