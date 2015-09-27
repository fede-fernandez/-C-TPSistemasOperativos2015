#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#include <commonsDeAsedio/cliente-servidor.h>

#include "estructurasSWAP.h"
#include "funcionesSWAP.h"
#include "huecosSWAP.h"
#include "particionSWAP.h"


#define MAXIMAS_CONEXIONES_ENTRANTES 1


tipoInstruccion* crearInstruccion(char ins,int pid,int pag,char* testo){
	tipoInstruccion* aux = malloc(sizeof(tipoInstruccion));

	aux->instruccion = ins;
	aux->pid = pid;
	aux->nroPagina = pag;
	aux->texto = strdup(testo);

	return aux;
}

int main(void) {

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("cfgSWAP");
	//FILE* particion = inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);
	t_list* listaDeHuecosUtilizados = inicializarListaDeHuecosUtilizados();
	FILE* particion = fopen("swap","r+");

//	int socketParaAdministrador = crearSocket();
//	asociarAPuerto(socketParaAdministrador,configuracion->puertoDeEscucha);
//	escucharConexiones(socketParaAdministrador,MAXIMAS_CONEXIONES_ENTRANTES);
//	int administradorMemoria = crearSocketParaAceptarSolicitudes(socketParaAdministrador);
//
//
//	char mensaje[30];
//	recibirMensaje(administradorMemoria,&mensaje,sizeof(mensaje));
//	printf("El comando recibido del Administrador de Memoria es: %s\n",mensaje);
//
//	liberarSocket(administradorMemoria);
//	liberarSocket(socketParaAdministrador);


	/****************PRUEBAS************************/

	//pid base cantidad
//	list_add(listaDeHuecosUtilizados,crearHuecoUtilizado(1,1,2));
//	list_add(listaDeHuecosUtilizados,crearHuecoUtilizado(2,4,2));
//	list_add(listaDeHuecosUtilizados,crearHuecoUtilizado(0,10,15));
//
//	printf("base para nuevo: %d",baseParaMProcSiTengoEspacioContiguo(listaDeHuecosUtilizados,20,64));


	reservarEspacio(listaDeHuecosUtilizados,0,10,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,particion);
	reservarEspacio(listaDeHuecosUtilizados,1,10,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,particion);
	reservarEspacio(listaDeHuecosUtilizados,2,10,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,particion);
	reservarEspacio(listaDeHuecosUtilizados,3,10,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,particion);
	reservarEspacio(listaDeHuecosUtilizados,4,10,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,particion);
	reservarEspacio(listaDeHuecosUtilizados,5,10,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,particion);

	reservarEspacio(listaDeHuecosUtilizados,15,1,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,particion);

	imprimirListaDeHuecos(listaDeHuecosUtilizados);

	liberarEspacio(listaDeHuecosUtilizados,2);
	liberarEspacio(listaDeHuecosUtilizados,5);

//	imprimirListaDeHuecos(listaDeHuecosUtilizados);
//
//	compactacionAlpha(listaDeHuecosUtilizados,particion,configuracion->tamanioDePagina);


	reservarEspacio(listaDeHuecosUtilizados,2000,20,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,particion);

//	escribirPagina(listaDeHuecosUtilizados,0,"hola",0,configuracion->tamanioDePagina,particion);
	char* paginaLeida = leerPagina(listaDeHuecosUtilizados,0,0,configuracion->tamanioDePagina,particion);
	imprimirListaDeHuecos(listaDeHuecosUtilizados);

	printf("%s",paginaLeida);




//	reservarEspacio(listaDeHuecosUtilizados,13,2,configuracion->cantidadDePaginas);
//	reservarEspacio(listaDeHuecosUtilizados,7,30,configuracion->cantidadDePaginas);




	puts("\n");

	//list_sort(listaDeHuecosUtilizados,(void*)baseMenor);

	//compactacionAlpha(listaDeHuecosUtilizados,particion,configuracion->tamanioDePagina);


	//imprimirListaDeHuecos(listaDeHuecosUtilizados);



	list_destroy_and_destroy_elements(listaDeHuecosUtilizados,(void*)destruirHuecoUtilizado);
	destruirConfigSWAP(configuracion);
	//fclose(particion);


	return EXIT_SUCCESS;
}

