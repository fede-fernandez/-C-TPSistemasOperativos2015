/*
 * funcionesSWAP.c
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

#include "funcionesSWAP.h"

////////////////////FUNCIONES PARA EL MANEJO DE ARCHIVO DE CONFIGURACION///////////////////////

tipoConfigSWAP* crearConfigSWAP(){
	tipoConfigSWAP* cfg = malloc(sizeof(tipoConfigSWAP));
	cfg->nombreDeSWAP = string_new();

	return cfg;
}

void destruirConfigSWAP (tipoConfigSWAP* cfg){
	free(cfg->nombreDeSWAP);
	free(cfg);
}

tipoConfigSWAP* cargarArchivoDeConfiguracionDeSWAP(char* rutaDelArchivoDeConfiguracion){

	t_config* archivoCfg = config_create(rutaDelArchivoDeConfiguracion);
	validarExistenciaDeArchivoDeConfiguracion(rutaDelArchivoDeConfiguracion);

	tipoConfigSWAP* cfg = crearConfigSWAP();

	validarErrorYAbortar(config_has_property(archivoCfg,PUERTO_ESCUCHA)
			&& config_has_property(archivoCfg,NOMBRE_SWAP)
			&& config_has_property(archivoCfg,CANTIDAD_PAGINAS)
			&& config_has_property(archivoCfg,TAMANIO_PAGINA)
			&& config_has_property(archivoCfg,RETARDO_COMPACTACION),
			"Las claves del archivo de configuracion no coinciden con las que requiere el SWAP");


	cfg->puertoDeEscucha = config_get_int_value(archivoCfg,PUERTO_ESCUCHA);
	cfg->nombreDeSWAP = string_duplicate(config_get_string_value(archivoCfg,NOMBRE_SWAP));
	cfg->cantidadDePaginas = config_get_int_value(archivoCfg,CANTIDAD_PAGINAS);
	cfg->retardoDeCompactacion = config_get_int_value(archivoCfg,RETARDO_COMPACTACION);
	cfg->tamanioDePagina = config_get_int_value(archivoCfg,TAMANIO_PAGINA);


	config_destroy(archivoCfg);

	return cfg;
}











