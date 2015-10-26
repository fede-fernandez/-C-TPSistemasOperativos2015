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
	t_list* listaTablaPaginas;
	int socketSWAP;
	int maximoSocket;
	tipoConfigMemoria* configuracion;
	fd_set* cpusATratar;
	bool* memoriaActiva;
	int socketCpus;
	t_list* listaAccesosAPaginasRAM;//Pongo
	//t_list* listaAccesosAPaginasTLB;//esto para
	char tipoDeAlgoritmoRAM;//que funcione
	char tipoDeAlgoritmoTLB;//fifo y lru
}tipoEstructuraMemoria;

typedef struct{
	int cantVecesAccedido;
	int pid;
	int nroPagina;
}tipoAccesosAPagina;

#define FIFO 1
#define LRU 2

typedef struct{
	int pid;
	int numeroDePagina;
	int posicionEnRAM;
}tipoTLB;

typedef struct{
	int pid;
	int paginasPedidas;
	int paginasAsignadas;
	t_list* frames;
}tipoTablaPaginas;

typedef struct{
	int numeroDePagina;
	bool modificado;
	int posicionEnRAM;
}tipoPagina;

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

char* traerPaginaDesdeRam(int direccion);

int buscarTabla(int pid);

int dondeEstaEnTabla(int nroPagina, int pid);

int dondeEstaEnTLB(int nroPagina, int pid);

void traerPaginaDesdeSwap(tipoInstruccion instruccion, tipoRespuesta* respuesta);

void quitarPaginasDeTLB(int pid);

int cantidadDePaginasAsignadas(int pid);

void quitarTabla(int pid);

void liberarPaginaDeRAM(int posicionEnRam);

//////////////////
//ESCRIBIR PAGINA
/////////////////

bool estaHabilitadaLaTLB();

void escribirPagina(tipoInstruccion instruccion,int cpuATratar);

/////////////////////
//FINALIZAR PROCESO
/////////////////////

bool instruccionASwapRealizada(tipoInstruccion* instruccion,tipoRespuesta** respuesta) ;

void quitarProceso(tipoInstruccion instruccion, int cpuaATratar);

void destruirProceso(int pid);


//**************************************************************************************************************
//**********************************FUNCIONES DE REEMPLAZO******************************************************
//**************************************************************************************************************

void agregarPaginaATLB(int nroPagina,int pid,int posicionEnRam);

void agregarPaginaATabla(int nroPagina,int pid,int posicionEnRam);//Agrega pagina a la tabla de pagina

bool agregarPagina(int nroPagina,int pid,char* pagina);//agrega a ram y a tabla de paginas

//int agregarPaginaARam(char* pagina);//Agrega la pagina a listaRAM y retorna la posicion dentro del t_list

int cualReemplazarRAM();

bool RAMLlena();

bool TLBLlena();

int cualReemplazarRAMFIFO();

int cualReemplazarRAMLRU();

int cualReemplazarTLBFIFO();

int cualReemplazarTLB();

void quitarPaginaDeTLB(int nroPagina,int pid);

void limpiarTLB();

void limpiarRam();

void volcarRamALog();

void limpiarTabla();

void limpiarListaAccesos();


#endif /* FUNCIONESMEMORIA_H_ */
