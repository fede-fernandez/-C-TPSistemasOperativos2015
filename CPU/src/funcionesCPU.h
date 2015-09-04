
#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_

#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>

#define IP_PLANIFICADOR "IP_PLANIFICADOR"
#define PUERTO_PLANIFICADOR "PUERTO_PLANIFICADOR"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define CANTIDAD_HILOS "CANTIDAD_HILOS"
#define RETARDO "RETARDO"

typedef struct{
	char* ipPlanificador;
	int puertoPlanificador;
	char* ipMemoria;
	int puertoMemoria;
	int cantidadDeHilos;
	int retardo;
}tipoConfigCPU;

tipoConfigCPU crearConfigCPU();

void destruirConfigCPU(tipoConfigCPU* estructuraDeConfiguracion);

void cargarArchivoDeConfiguracionDeCPU(t_config* archivoDeConfiguracion,tipoConfigCPU* estructuraDeConfiguracion);



#endif /* FUNCIONESCPU_H_ */
