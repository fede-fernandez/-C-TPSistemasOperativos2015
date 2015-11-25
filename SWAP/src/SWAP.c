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
#define INFINITO 1


//tipoInstruccion* crearInstruccion(char ins,int pid,int pag,char* testo){
//	tipoInstruccion* aux = malloc(sizeof(tipoInstruccion));
//
//	aux->instruccion = ins;
//	aux->pid = pid;
//	aux->nroPagina = pag;
//	aux->texto = strdup(testo);
//
//	return aux;
//}

int main(void) {

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("cfgSWAP");
	t_list* listaDeHuecosUtilizados = inicializarListaDeHuecosUtilizados();
	t_log* logger = crearLoggerParaSeguimiento("logSWAP","Administrador de SWAP");

	inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);

	tipoInstruccion* instruccionAEjecutar;
	tipoRespuesta* respuestaParaMemoria;

	bool finalizarProceso = false;

	char* textoALogear = string_new();
	string_append(&textoALogear,"Inicio de ejecucion de proceso SWAP");
	logearSeguimiento(textoALogear,logger);



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


		if (instruccionAEjecutar->instruccion == 'h') {
			finalizarProceso = true;
			destruirTipoInstruccion(instruccionAEjecutar);
		}
		else {
			respuestaParaMemoria = ejecutarInstruccion(instruccionAEjecutar,listaDeHuecosUtilizados,configuracion,logger);

			enviarRespuesta(socketParaRecibirInstrucciones,respuestaParaMemoria);
			printf("Respuesta enviada\n\n");

			imprimirListaDeHuecos(listaDeHuecosUtilizados);

			destruirTipoRespuesta(respuestaParaMemoria);
			destruirTipoInstruccion(instruccionAEjecutar);
		}
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

	free(textoALogear);


	return EXIT_SUCCESS;
}

