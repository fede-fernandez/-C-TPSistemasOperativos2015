/*
 * estructurasSWAP.h
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#ifndef ESTRUCTURASSWAP_H_
#define ESTRUCTURASSWAP_H_



//------------CONFIGURACION-------------

typedef struct{
	int puertoDeEscucha;
	char* nombreDeSWAP;
	int cantidadDePaginas;
	int tamanioDePagina;
	int retardoDeCompactacion;
}tipoConfigSWAP;

//------------CONTROL DE HUECOS-------------

typedef struct{
	int pid;
	int baseDeMProc;
	int cantidadDePaginasQueOcupa;
}tipoHuecoUtilizado;

typedef struct{
	int baseDeMProc;
	int cantidadDePaginasQueOcupa;
}tipoHuecoLibre;

#endif /* ESTRUCTURASSWAP_H_ */
