#ifndef COMMONSDEASEDIO_CONFIGURACION_H_
#define COMMONSDEASEDIO_CONFIGURACION_H_

#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>


int obtenerCampoDeConfiguracion(char** campoObtenido,char* campoBuscado,t_config* archivoDeConfiguracion);
void validarExistenciaDeArchivoDeConfiguracion(char* rutaDelArchivoDeConfiguracion);

#endif /* COMMONSDEASEDIO_CONFIGURACION_H_ */
