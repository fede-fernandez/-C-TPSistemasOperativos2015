/*
 * funcionesSWAP.c
 *
 *  Created on: 4/9/2015
 *      Author: utnso
 */

#include "funcionesSWAP.h"

////////////////////FUNCIONES PARA EL MANEJO DE ARCHIVO DE CONFIGURACION///////////////////////

tipoConfigSWAP* crearConfigSWAP(){
	tipoConfigSWAP* cfg = malloc(sizeof(tipoConfigSWAP));
	cfg->nombreDeSWAP = string_new();

	return cfg;
}

void destruirConfigSWAP (tipoConfigSWAP* cfg){
	free(cfg->nombreDeSWAP);
	free(cfg);
}

tipoConfigSWAP* cargarArchivoDeConfiguracionDeSWAP(char* rutaDelArchivoDeConfiguracion){

	t_config* archivoCfg = config_create(rutaDelArchivoDeConfiguracion);

	tipoConfigSWAP* cfg = crearConfigSWAP();

	validarErrorYAbortar(config_has_property(archivoCfg,PUERTO_ESCUCHA)
			&& config_has_property(archivoCfg,NOMBRE_SWAP)
			&& config_has_property(archivoCfg,CANTIDAD_PAGINAS)
			&& config_has_property(archivoCfg,TAMANIO_PAGINA)
			&& config_has_property(archivoCfg,RETARDO_COMPACTACION),
			"Las claves del archivo de configuracion no coinciden con las que requiere el SWAP");


	cfg->puertoDeEscucha = config_get_int_value(archivoCfg,PUERTO_ESCUCHA);
	cfg->nombreDeSWAP = string_duplicate(config_get_string_value(archivoCfg,NOMBRE_SWAP));
	cfg->cantidadDePaginas = config_get_int_value(archivoCfg,CANTIDAD_PAGINAS);
	cfg->retardoDeCompactacion = config_get_int_value(archivoCfg,RETARDO_COMPACTACION);


	config_destroy(archivoCfg);

	return cfg;
}


//////////////FUNCIONES PARA EL ARCHIVO DE PARTICION///////////////////////


//FILE* inicializarParticion(char* nombreDeParticion,int tamanioDePagina,int cantidadDePaginas){
//
//
//	int tamanioDeArchivo = 50;//tamanioDePagina*cantidadDePaginas;
//	char* instruccion = string_new();
//	void* mapeo;
//	FILE* particion;
//
//
//	//sprintf(instruccion,"truncate -s %d %s",tamanioDeArchivo,nombreDeParticion);
//	//system(instruccion);
//
//	particion = fopen(nombreDeParticion,"w+");
//
//	fprintf(particion,"%s",string_repeat('\0',20000000));
//
//	//mapeo = mapearArchivoCompleto(particion);
//	//sprintf((char*)mapeo,"%s",string_repeat('a',tamanioDeArchivo));
//	//liberarMemoriaDeArchivoCompletoMapeado(particion,mapeo);
//
//	return particion;
//}


/////////////////////LISTAS DE HUECOS///////////////////////







tipoHuecoLibre* crearHuecoLibre(int inicio,int cantidadDePaginas){

	tipoHuecoLibre* aux = malloc(sizeof(tipoHuecoLibre));
	aux->baseDeMProc = inicio;
	aux->cantidadDePaginasQueOcupa = cantidadDePaginas;

	return aux;
}

void destruirHuecoLibre(tipoHuecoLibre* huecoLibre){
	free(huecoLibre);
}



t_list* inicializarListaDeHuecosLibres(int cantidadDePaginas){

	t_list*	listaDeHuecosLibres = list_create();
	list_add(listaDeHuecosLibres,crearHuecoLibre(0,cantidadDePaginas));
	return listaDeHuecosLibres;
}


int espacioDisponible(t_list* listaDeHuecosLibres,int tamanioDePagina){
	int espacioDisponible = 0;
	int i;
	tipoHuecoLibre* aux = malloc(sizeof(tipoHuecoLibre));

	for (i = 0; i < list_size(listaDeHuecosLibres); i++) {
		aux = list_get(listaDeHuecosLibres,i);
		espacioDisponible+= (tamanioDePagina*(aux->cantidadDePaginasQueOcupa));

	}

	destruirHuecoLibre(aux);
	return espacioDisponible;
}




