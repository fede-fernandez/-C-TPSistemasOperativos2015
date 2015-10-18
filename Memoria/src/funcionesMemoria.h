/*
 * funcionesMemoria.h
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

#ifndef FUNCIONESMEMORIA_H_
#define FUNCIONESMEMORIA_H_

#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>
#include <commons/collections/list.h>


#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define IP_SWAP "IP_SWAP"
#define PUERTO_SWAP "PUERTO_SWAP"
#define MAXIMO_MARCOS_POR_PROCESO "MAXIMO_MARCOS_POR_PROCESO"
#define CANTIDAD_MARCOS "CANTIDAD_MARCOS"
#define TAMANIO_MARCO "TAMANIO_MARCO"
#define ENTRADAS_TLB "ENTRADAS_TLB"
#define TLB_HABILITADA "TLB_HABILITADA"
#define RETARDO_MEMORIA "RETARDO_MEMORIA"

/*******************ESTRUCTURAS*************************/

typedef struct{
	int puertoDeEscucha;
	char* ipSWAP;
	int puertoSWAP;
	int maximoDeMarcosPorProceso;
	int cantidadDeMarcos;
	int tamanioDeMarco;
	int entradasDeTLB;
	char* TLBHabilitada;
	int retardoDeMemoria;
}tipoConfigMemoria;

typedef struct{
	t_list* listaRAM;
	t_list* listaTLB;
	int socketSWAP;
	int maximoSocket;
	tipoConfigMemoria* configuracion;
	fd_set* cpusATratar;
	t_list* administradorPaginas;
	bool* memoriaActiva;
	int socketCpus;
}tipoEstructuraMemoria;

typedef struct{
	int pid;
	int numeroDePagina;
	int posicionEnRAM;
}tipoTLB;

typedef struct{
	int pid;
	int numeroDePagina;
	bool modificado;
	char* contenido;
}tipoRAM;

typedef struct{
	int pid;
	int paginasPedidas;
	int paginasAsignadas;
}tipoAdministracionPaginas;


tipoEstructuraMemoria* datosMemoria;

void setearEstructuraMemoria(tipoEstructuraMemoria*);

/***************************FUNCIONES*********************************************/

tipoConfigMemoria* crearConfigMemoria();

void destruirConfigMemoria(tipoConfigMemoria* estructuraDeConfiguracion);

tipoConfigMemoria* cargarArchivoDeConfiguracionDeMemoria(char* rutaDelArchivoDeConfiguracion);

void tratarPeticion(int cpuAtendida);

void tratarPeticiones();

/**************INSTRUCCIONES*******************/

//////////////////
//INICIAR
/////////////////

void reservarMemoriaParaProceso(tipoInstruccion instruccion, int cpuATratar);

bool puedoReservarEnSWAP(tipoInstruccion instruccion, tipoRespuesta* respuesta);

//////////////////
//LEER PAGINA
/////////////////

void enviarPaginaPedidaACpu(tipoInstruccion instruccion, int cpuATratar);

void liberarEstructuraRAM(int posicionEnRam);

void quitarPaginasDeRAM(int pid);

int cantidadDePaginasAsignadas(int pid);

void quitarPaginasDeTLB(int pid);

int traerPaginaDesdeSwap(tipoInstruccion instruccion, tipoRespuesta* respuesta);

int dondeEstaEnRam(int nroPagina, int pid);

int dondeEstaEnTLB(int nroPagina, int pid);

//////////////////
//ESCRIBIR PAGINA
/////////////////

bool estaHabilitadaLaTLB();

void escribirPagina(tipoInstruccion instruccion,int cpuATratar);

/////////////////////
//FINALIZAR PROCESO
/////////////////////

bool instruccionASwapRealizada(tipoInstruccion instruccion,tipoRespuesta* respuesta) ;

void quitarProceso(tipoInstruccion instruccion, int cpuaATratar);

void destruirProceso(int pid);

void quitarAdministracionDePaginas(int pid);

#endif /* FUNCIONESMEMORIA_H_ */
