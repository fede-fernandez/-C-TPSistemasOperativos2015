
#include "funcionesCPU.h"

tipoConfigCPU* crearConfigCPU(){
	tipoConfigCPU* cfg = malloc(sizeof(tipoConfigCPU));
	cfg->ipPlanificador = string_new();
	cfg->ipMemoria = string_new();

	return cfg;
}

void destruirConfigCPU(tipoConfigCPU* cfg){
	free(cfg->ipPlanificador);
	free(cfg->ipMemoria);
	free(cfg);
}

void cargarArchivoDeConfiguracionDeCPU(t_config* archivoCfg,tipoConfigCPU* cfg){
	if (config_has_property(archivoCfg,IP_PLANIFICADOR) && config_has_property(archivoCfg,PUERTO_PLANIFICADOR) && config_has_property(archivoCfg,IP_MEMORIA) && config_has_property(archivoCfg,PUERTO_MEMORIA) && config_has_property(archivoCfg,CANTIDAD_HILOS) && config_has_property(archivoCfg,RETARDO)) {

		cfg->ipPlanificador = string_duplicate(config_get_string_value(archivoCfg,IP_PLANIFICADOR));
		cfg->puertoPlanificador = config_get_int_value(archivoCfg,PUERTO_PLANIFICADOR);
		cfg->ipMemoria = string_duplicate(config_get_string_value(archivoCfg,IP_MEMORIA));
		cfg->puertoMemoria = config_get_int_value(archivoCfg,PUERTO_MEMORIA);
		cfg->cantidadDeHilos = config_get_int_value(archivoCfg,CANTIDAD_HILOS);
		cfg->retardo = config_get_int_value(archivoCfg,RETARDO);
	}
}
