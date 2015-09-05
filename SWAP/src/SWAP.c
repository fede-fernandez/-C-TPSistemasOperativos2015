#include <stdio.h>
#include <stdlib.h>
#include "funcionesSWAP.h"

int main(void) {

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("/home/utnso/Escritorio/cfgSWAP");



	destruirConfigSWAP(configuracion);

	return EXIT_SUCCESS;
}
