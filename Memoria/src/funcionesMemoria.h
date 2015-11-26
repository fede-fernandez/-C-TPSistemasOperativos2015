/*
 * funcionesMemoria.h
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

#ifndef FUNCIONESMEMORIA_H_
#define FUNCIONESMEMORIA_H_

#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <commonsDeAsedio/error.h>
#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>
#include <commons/collections/list.h>
#include "configuracion.h"
#include "algoritmos.h"
#include "validaciones.h"

tipoEstructuraMemoria* datosMemoria;

void setearEstructuraMemoria(tipoEstructuraMemoria*);

/*###############################FUNCIONES PRINCIPALES##########################################*/

void tratarPeticion(int cpuAtendida);

tipoRespuesta* iniciar(tipoInstruccion* instruccion);

tipoRespuesta* leerPagina(tipoInstruccion* instruccion);

tipoRespuesta* escribirPagina(tipoInstruccion* instruccion);

tipoRespuesta* quitarProceso(tipoInstruccion* instruccion);//Destruye un proceso y le avisa a swap

tipoRespuesta* finalizarMemoria(tipoInstruccion* instruccion);

/*###############################BUSQUEDAS##########################################*/

int buscarTabla(int pid);//Busca la posicion de la tabla de paginas correspondiente a pid

int buscarPaginaEnTabla(int nroPagina,int pid);//Si la pagina es encontrada retorna la pos en ram, si no retorna el estado de presencia

int buscarPaginaEnTLB(int nroPagina,int pid);//Retorna la posicion en ram accediendo a traves de la TLB

int buscarPagina(int nroPagina,int pid);//Encuentra una pagina donde quiera que este

int traerPaginaDesdeSwap(tipoInstruccion* instruccion, tipoRespuesta** respuesta);//Busca una pagina en swap

char* traerPaginaDesdeRam(int direccion);//Retorna el contenido de la pagina

tipoTablaPaginas* traerTablaDePaginas(int pid);//Retorna la tabla de paginas correspondiente a pid

int dondeEstaPaginaEnTLB(int nroPagina,int pid);//Busca en que posicion de la TLB se encuentra determinada instancia

int buscarHuecoRAM();//Retorna la posicion del primer hueco de RAM que encuentra

/*####################################MODIFICACIONES###################################################*/

void modificarDatosDePagina(int nroPagina,int pid,int posicionEnRam,int presente,bool uso,bool modificado);

void modificarUso(int nroPagina,int pid,bool uso);//modifica el bit de uso

void modificarModificado(int nroPagina,int pid,bool modificado);//modifica el bit de modificacion

void modificarPagina(int nroPagina,int pid,int posicionDePag,char* texto);//Modifica contenido de una pagina

void aumentarPaginasAsignadas(int pid);//Aumenta la cantidad de paginas asignadas a un proceso

void setearHuecoEnListaHuecosRAM(int posicion,bool estado);//Setea si hay o no un hueco en ram

void aumentarAccesosAProceso(int pid);//Aumenta el acceso a un determinado proceso (esto es para hacer lo de la tasa de aciertos)

void aumentarPageFaults(int pid);

/*####################################CREACIONES O AGREGADOS###################################################*/

void agregarPaginaATLB(int nroPagina,int pid,int posicionEnRam);

void agregarATablaDePaginas(tipoTablaPaginas* tablaDePaginas,int nroPagina,int posEnRAM,char presente,bool usado,bool modificado);

tipoPagina* crearPagina(int nroPagina,int posEnRAM,char presente,bool usado,bool modificado);

void inicializarPaginas(tipoTablaPaginas* tablaDePaginasNueva);

void iniciarListaHuecosRAM();//Inicializa la RAM con huecos

int agregarPagina(int nroPagina,int pid,char* contenido);

char* generarPaginaCompleta(char* pagina);

char* crearPaginaManqueada();

/*####################################LIMPIEZA DE LISTAS###################################################*/

void quitarDeTLB(int nroPagina,int pid);//Quita pagina de TLB

void quitarTablaDePaginas(int pid);//Borra toda la tabla de paginas (incluido accesos)

void quitarPaginaDeRam(int posicion);//Quita una pagina de la ram (mete una cadena vacia y setea el hueco en la lista de huecos)

void limpiarRam();//Quita todas las pags de ram y las lleva a swap (si es que estan modificadas)

void llevarPaginasASwap(tipoTablaPaginas* tablaDePaginas);//Lleva todas las paginas de la tabla modificadas a swap

void limpiarTLB();//Deja la tlb vacia

void destruirProceso(int pid);//Borra el proceso del sistema

/*####################################OTROS###################################################*/

double porcentajeDePageFaults(int pid);

double tasaAciertosTLB();//Retorna la tasa de aciertos de tlb en %

void dormirPorAccesoARAM();

void llevarPaginaASwap(int nroPaginaAReemplazar,int pid,int posicionEnRam);

bool puedoReservarEnSWAP(tipoInstruccion* instruccion, tipoRespuesta** respuesta) ;

bool instruccionASwapRealizada(tipoInstruccion* instruccion,tipoRespuesta** respuesta) ;

void volcarRamALog();

#endif /* FUNCIONESMEMORIA_H_ */
