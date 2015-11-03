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

void validarExistenciaDeArchivoDeConfiguracion(char* rutaDelArchivoDeConfiguracion)
{
	FILE* archivoDeConfiguracion = fopen(rutaDelArchivoDeConfiguracion, "r");
	if(archivoDeConfiguracion == NULL)
	{
		perror("El archivo de configuracion no existe o esta vacio.");
		fclose(archivoDeConfiguracion);
		abort();
	}
	fclose(archivoDeConfiguracion);
}
