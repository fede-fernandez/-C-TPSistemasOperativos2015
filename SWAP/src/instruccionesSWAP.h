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


#include <commonsDeAsedio/estructuras.h>
#include <commons/log.h>
#include <commons/collections/list.h>


tipoRespuesta* ejecutarInstruccion(tipoInstruccion* instruccion,t_list* listaDeHuecos,tipoConfigSWAP* configuracion, t_log* logger, bool* finalizarProceso);

#endif /* INSTRUCCIONESSWAP_H_ */
