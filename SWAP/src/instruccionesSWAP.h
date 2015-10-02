/*
 * instruccionesSWAP.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef INSTRUCCIONESSWAP_H_
#define INSTRUCCIONESSWAP_H_

#include "huecosSWAP.h"
#include "particionSWAP.h"


#include <commonsDeAsedio/estructuras.h>


tipoRespuesta* ejecutarInstruccion(tipoInstruccion* instruccion,t_list* listaDeHuecos,tipoConfigSWAP* configuracion);

#endif /* INSTRUCCIONESSWAP_H_ */
