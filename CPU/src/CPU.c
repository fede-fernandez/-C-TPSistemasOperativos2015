#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"

int main(void) {

	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU("/home/utnso/Escritorio/cfgCPU");//ruta de ejemplo



	destruirConfigCPU(configuracion);
	return EXIT_SUCCESS;
}
