#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>

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

	inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);

	tipoInstruccion* instruccionAEjecutar;
	tipoRespuesta* respuestaParaMemoria;

	//FILE* particion = fopen("swap","r+");

/*************Sockets***************/
	int socketParaAdministrador = crearSocket();
	asociarAPuerto(socketParaAdministrador,configuracion->puertoDeEscucha);

	printf("Escuchando conexiones...\n");
	escucharConexiones(socketParaAdministrador,MAXIMAS_CONEXIONES_ENTRANTES);
	int socketParaRecibirInstrucciones = crearSocketParaAceptarSolicitudes(socketParaAdministrador);
	printf("Conexion establecida\n\n");



/***********Recibir instrucciones y ejecutarlas***********/
	//while(INFINITO){

		printf("Esperando instruccion\n");
		instruccionAEjecutar = recibirInstruccion(socketParaRecibirInstrucciones);
		printf("Instruccion recibida\n");

		respuestaParaMemoria = ejecutarInstruccion(instruccionAEjecutar,listaDeHuecosUtilizados,configuracion);

		enviarRespuesta(socketParaRecibirInstrucciones,respuestaParaMemoria);
		printf("Respuesta enviada\n\n");

		destruirTipoRespuesta(respuestaParaMemoria);
		destruirTipoInstruccion(instruccionAEjecutar);

		imprimirListaDeHuecos(listaDeHuecosUtilizados);
	//}


		tipoInstruccion* instruccionAEjecutar2;
		tipoRespuesta* respuestaParaMemoria2;

		printf("Esperando instruccion 2\n");
		instruccionAEjecutar2 = recibirInstruccion(socketParaRecibirInstrucciones);
		printf("Instruccion 2 recibida \n");

		respuestaParaMemoria2 = ejecutarInstruccion(instruccionAEjecutar2,listaDeHuecosUtilizados,configuracion);

		enviarRespuesta(socketParaRecibirInstrucciones,respuestaParaMemoria2);
		printf("Respuesta 2 enviada\n\n");

		imprimirListaDeHuecos(listaDeHuecosUtilizados);


/********Liberar memoria*************/


	printf("Libero sockets\n");
	liberarSocket(socketParaRecibirInstrucciones);
	liberarSocket(socketParaAdministrador);

	printf("Libero estructuras\n");
	list_destroy_and_destroy_elements(listaDeHuecosUtilizados,(void*)destruirHuecoUtilizado);
	destruirConfigSWAP(configuracion);



	return EXIT_SUCCESS;
}

