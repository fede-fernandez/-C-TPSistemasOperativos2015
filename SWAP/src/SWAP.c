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

	t_log* logger = crearLoggerParaSeguimiento("logSWAP","Administrador de SWAP");
	logearSeguimiento("Inicio de ejecucion de proceso SWAP",logger);

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("cfgSWAP");
	t_list* listaDeHuecosUtilizados = inicializarListaDeHuecosUtilizados();

	inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);

	tipoInstruccion* instruccionAEjecutar;
	tipoRespuesta* respuestaParaMemoria;

	bool finalizarProceso = false;



/*************Sockets***************/
	int socketParaAdministrador = crearSocket();
	asociarAPuerto(socketParaAdministrador,configuracion->puertoDeEscucha);

	printf("Escuchando conexiones...\n");
	escucharConexiones(socketParaAdministrador,MAXIMAS_CONEXIONES_ENTRANTES);
	int socketParaRecibirInstrucciones = crearSocketParaAceptarSolicitudes(socketParaAdministrador);
	printf("Conexion establecida\n\n");


	//int i = 1;
/***********Recibir instrucciones y ejecutarlas***********/
	while(!finalizarProceso){

		printf("Esperando instruccion\n");
		instruccionAEjecutar = recibirInstruccion(socketParaRecibirInstrucciones);
		printf("Instruccion recibida\n");


		respuestaParaMemoria = ejecutarInstruccion(instruccionAEjecutar,listaDeHuecosUtilizados,configuracion,logger, &finalizarProceso);

		enviarRespuesta(socketParaRecibirInstrucciones,respuestaParaMemoria);
		printf("Respuesta enviada\n\n");

		imprimirListaDeHuecos(listaDeHuecosUtilizados);

		destruirTipoRespuesta(respuestaParaMemoria);
		destruirTipoInstruccion(instruccionAEjecutar);

	//	i++;
	}




/********Liberar memoria*************/


	printf("Libero sockets\n");
	liberarSocket(socketParaRecibirInstrucciones);
	liberarSocket(socketParaAdministrador);

	printf("Libero estructuras\n");
	list_destroy_and_destroy_elements(listaDeHuecosUtilizados,(void*)destruirHuecoUtilizado);
	destruirConfigSWAP(configuracion);
	destruirLogger(logger);



	return EXIT_SUCCESS;
}

