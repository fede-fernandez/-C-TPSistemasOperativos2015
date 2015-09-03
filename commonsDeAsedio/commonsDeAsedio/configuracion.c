#include "configuracion.h"

int obtenerCampoDeConfiguracion(char** campoObtenido,char* campoBuscado,t_config* archivoDeConfiguracion){
	*campoObtenido=string_new();

	if(config_has_property(archivoDeConfiguracion,campoBuscado)){
		string_append(campoObtenido,config_get_string_value(archivoDeConfiguracion,campoBuscado));
		return 0;
	}
	else{
		return -1;
	}
}
