#include <stdio.h>
#include <stdlib.h>
#include "funcionesMemoria.h"

int main(void) {

	tipoConfigMemoria* configuracion = cargarArchivoDeConfiguracionDeMemoria("/home/utnso/Escritorio/cfgMemoria");



	destruirConfigMemoria(configuracion);
	return EXIT_SUCCESS;
}
