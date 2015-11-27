/*
 * instruccionesSWAP.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef INSTRUCCIONESSWAP_H_
#define INSTRUCCIONESSWAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "huecosSWAP.h"
#include "particionSWAP.h"
#include "funcionesSWAP.h"


#include <commonsDeAsedio/estructuras.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>


tipoRespuesta* ejecutarInstruccion(tipoInstruccion* instruccion,t_list* listaDeHuecosUtilizados,t_list* listaDeHuecosLibres,tipoConfigSWAP* configuracion, t_log* logger, bool* finalizarProceso);

tipoRespuesta* reservarEspacio(t_list* listaDeHuecosUtilizados,t_list* listaDeHuecosLibres,int pidProcesoNuevo, int cantPaginasSolicitadas,int cantDePaginasDeSWAP,int tamanioDePagina, char* particion, int retardoDeCompactacion, t_log* logger);
tipoRespuesta* liberarEspacio(t_list* listaDeHuecosUtilizados,t_list* listaDeHuecosLibres,int pidProceso, int tamanioDePagina, t_log* logger, char* rutaDeParticion);
tipoRespuesta* leerPagina(t_list* listaDeHuecosUtilizados,int pidProceso,int dirLogicaDePagina,int tamanioDePagina,char* particion, t_log* logger, int retardoDeLectura);
tipoRespuesta* escribirPagina(t_list* listaDeHuecosUtilizados,int pidProceso,char* contenidoAEscribir,int dirLogicaDePagina,int tamanioDePagina, char* particion, t_log* logger,int retardoDeEscritura);
tipoRespuesta* finalizarProcesoSWAP(bool* finalizarProceso);

#endif /* INSTRUCCIONESSWAP_H_ */
