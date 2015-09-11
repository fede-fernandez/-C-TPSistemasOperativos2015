#include <stdio.h>
#include <stdlib.h>
#include "funcionesSWAP.h"

int main(void) {

	tipoConfigSWAP* configuracion = cargarArchivoDeConfiguracionDeSWAP("/home/utnso/Escritorio/cfgSWAP");

	//FILE* particion = inicializarParticion(configuracion->nombreDeSWAP,configuracion->tamanioDePagina,configuracion->cantidadDePaginas);

	t_list* listaDeHuecosLibres = inicializarListaDeHuecosLibres(configuracion->cantidadDePaginas);








	destruirConfigSWAP(configuracion);
	//fclose(particion);

	return EXIT_SUCCESS;
}
