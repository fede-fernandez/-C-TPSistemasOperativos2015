/*
 * configuracion.c
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#include "configuracion.h"


void destruirConfigMemoria(tipoConfigMemoria* estructuraDeConfiguracion) {
	free(estructuraDeConfiguracion->ipSWAP);
	free(estructuraDeConfiguracion->TLBHabilitada);
	free(estructuraDeConfiguracion);
}

tipoConfigMemoria* crearConfigMemoria() {
	tipoConfigMemoria* cfg = malloc(sizeof(tipoConfigMemoria));
	cfg->ipSWAP = string_new();
	cfg->TLBHabilitada = string_new();
	cfg->algoritmoRAM = string_new();

	return cfg;
}

/*********************ESTRUCTURAS*************************/

tipoConfigMemoria* cargarArchivoDeConfiguracionDeMemoria(
		char* rutaDelArchivoDeConfiguracion) {

	t_config* archivoCfg = config_create(rutaDelArchivoDeConfiguracion);
	tipoConfigMemoria* cfg = crearConfigMemoria();

	validarErrorYAbortar(
			config_has_property(archivoCfg, PUERTO_ESCUCHA)
					&& config_has_property(archivoCfg, IP_SWAP)
					&& config_has_property(archivoCfg, PUERTO_SWAP)
					&& config_has_property(archivoCfg,
					MAXIMO_MARCOS_POR_PROCESO)
					&& config_has_property(archivoCfg, CANTIDAD_MARCOS)
					&& config_has_property(archivoCfg, TAMANIO_MARCO)
					&& config_has_property(archivoCfg, ENTRADAS_TLB)
					&& config_has_property(archivoCfg, TLB_HABILITADA)
					&& config_has_property(archivoCfg, RETARDO_MEMORIA)
					&& config_has_property(archivoCfg, ALGORITMO_RAM),
			"Las claves del archivo de configuracion no coinciden con las que requiere el Administrador de Memoria.");

	cfg->puertoDeEscucha = config_get_int_value(archivoCfg, PUERTO_ESCUCHA);
	cfg->ipSWAP = string_duplicate(
			config_get_string_value(archivoCfg, IP_SWAP));
	cfg->puertoSWAP = config_get_int_value(archivoCfg, PUERTO_SWAP);
	cfg->maximoDeMarcosPorProceso = config_get_int_value(archivoCfg,
	MAXIMO_MARCOS_POR_PROCESO);
	cfg->cantidadDeMarcos = config_get_int_value(archivoCfg, CANTIDAD_MARCOS);
	cfg->tamanioDeMarco = config_get_int_value(archivoCfg, TAMANIO_MARCO);
	cfg->entradasDeTLB = config_get_int_value(archivoCfg, ENTRADAS_TLB);
	cfg->TLBHabilitada = string_duplicate(
			config_get_string_value(archivoCfg, TLB_HABILITADA));
	cfg->retardoDeMemoria = config_get_double_value(archivoCfg, RETARDO_MEMORIA);
	cfg->algoritmoRAM = string_duplicate(
			config_get_string_value(archivoCfg, ALGORITMO_RAM));


	config_destroy(archivoCfg);

	return cfg;
}
