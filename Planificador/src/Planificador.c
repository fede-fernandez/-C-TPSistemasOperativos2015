#include <stdio.h>
#include <stdlib.h>
#include "funcionesPlanificador.h"

int main(void) {

	tipoConfigPlanificador* configuracion = cargarArchivoDeConfiguracionDelPlanificador("/home/utnso/Escritorio/cfgPlanificador");




	destruirConfigPlanificador(configuracion);

	return EXIT_SUCCESS;
}
