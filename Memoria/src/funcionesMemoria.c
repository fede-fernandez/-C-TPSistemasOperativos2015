#include "funcionesMemoria.h"
#include <commonsDeAsedio/estructuras.h>

void destruirConfigMemoria(tipoConfigMemoria* estructuraDeConfiguracion) {
	free(estructuraDeConfiguracion->ipSWAP);
	free(estructuraDeConfiguracion->TLBHabilitada);
	free(estructuraDeConfiguracion);
}

tipoConfigMemoria* crearConfigMemoria() {
	tipoConfigMemoria* cfg = malloc(sizeof(tipoConfigMemoria));
	cfg->ipSWAP = string_new();
	cfg->TLBHabilitada = string_new();

	return cfg;
}

/*********************ESTRUCTURAS*************************/

tipoConfigMemoria* cargarArchivoDeConfiguracionDeMemoria(
		char* rutaDelArchivoDeConfiguracion) {

	t_config* archivoCfg = config_create(rutaDelArchivoDeConfiguracion);
	tipoConfigMemoria* cfg = crearConfigMemoria();

	validarErrorYAbortar(
			config_has_property(archivoCfg, PUERTO_ESCUCHA)
					&& config_has_property(archivoCfg, IP_SWAP)
					&& config_has_property(archivoCfg, PUERTO_SWAP)
					&& config_has_property(archivoCfg,
					MAXIMO_MARCOS_POR_PROCESO)
					&& config_has_property(archivoCfg, CANTIDAD_MARCOS)
					&& config_has_property(archivoCfg, TAMANIO_MARCO)
					&& config_has_property(archivoCfg, ENTRADAS_TLB)
					&& config_has_property(archivoCfg, TLB_HABILITADA)
					&& config_has_property(archivoCfg, RETARDO_MEMORIA),
			"Las claves del archivo de configuracion no coinciden con las que requiere el Administrador de Memoria.");

	cfg->puertoDeEscucha = config_get_int_value(archivoCfg, PUERTO_ESCUCHA);
	cfg->ipSWAP = string_duplicate(
			config_get_string_value(archivoCfg, IP_SWAP));
	cfg->puertoSWAP = config_get_int_value(archivoCfg, PUERTO_SWAP);
	cfg->maximoDeMarcosPorProceso = config_get_int_value(archivoCfg,
	MAXIMO_MARCOS_POR_PROCESO);
	cfg->cantidadDeMarcos = config_get_int_value(archivoCfg, CANTIDAD_MARCOS);
	cfg->tamanioDeMarco = config_get_int_value(archivoCfg, TAMANIO_MARCO);
	cfg->entradasDeTLB = config_get_int_value(archivoCfg, ENTRADAS_TLB);
	cfg->TLBHabilitada = string_duplicate(
			config_get_string_value(archivoCfg, TLB_HABILITADA));
	cfg->retardoDeMemoria = config_get_int_value(archivoCfg, RETARDO_MEMORIA);

	config_destroy(archivoCfg);

	return cfg;
}

void setearEstructuraMemoria(tipoEstructuraMemoria* datos) {

	datosMemoria = datos;

	datosMemoria->administradorPaginas = list_create();

	datosMemoria->listaRAM = list_create();

	datosMemoria->listaTLB = list_create();
}

/************************FUNCIONES********************************/

void tratarPeticion(int cpuAtendida) {

	tipoInstruccion* instruccion = recibirInstruccion(cpuAtendida);

	switch (instruccion->instruccion) {
	case INICIAR:
		reservarMemoriaParaProceso(*instruccion, cpuAtendida);
		break;

	case LEER:
		enviarPaginaPedidaACpu(*instruccion,cpuAtendida);
		break;

	case ESCRIBIR:
			escribirPagina(*instruccion,cpuAtendida);
		break;

	case FINALIZAR:
		quitarProceso(*instruccion, cpuAtendida);
		break;
	}
}

void tratarPeticiones() {

	int var;
	for (var = 1; var <= datosMemoria->maximoSocket; ++var) {
		if (FD_ISSET(var, datosMemoria->cpusATratar)&&var!=datosMemoria->socketCpus)
			tratarPeticion(var);
	}
}

/**************INSTRUCCIONES*******************/

//////////////////
//INICIAR
/////////////////
void reservarMemoriaParaProceso(tipoInstruccion instruccion, int cpuATratar) {

	tipoRespuesta respuesta;

	if (puedoReservarEnSWAP(instruccion, &respuesta)) {

		printf("pude reservar en swap!!\n");

		tipoAdministracionPaginas* instanciaDeAdministracion = malloc(sizeof(tipoAdministracionPaginas));

		printf("pedi instancia de admin\n");

		instanciaDeAdministracion->pid = instruccion.pid;

		instanciaDeAdministracion->paginasAsignadas = 0;

		instanciaDeAdministracion->paginasPedidas = instruccion.nroPagina;

		printf("cree instancia de administracion..\n");

		list_add(datosMemoria->administradorPaginas, instanciaDeAdministracion);

		printf("agregue pagina a admin de paginas");
	}

	enviarRespuesta(cpuATratar, respuesta);
}

bool puedoReservarEnSWAP(tipoInstruccion instruccion, tipoRespuesta* respuesta) {

	return instruccionASwapRealizada(instruccion, respuesta);
}

//////////////////
//LEER PAGINA
/////////////////

void enviarPaginaPedidaACpu(tipoInstruccion instruccion, int cpuATratar) {

	tipoRespuesta* respuesta;

	char* contenidoDePagina;

	int posicionDePagEnRam = -1;

	tipoRAM* instanciaRamActual;

	int posicionDePagEnTLB = dondeEstaEnTLB(instruccion.nroPagina,instruccion.pid);

	if (posicionDePagEnTLB >= 0) {

		respuesta->respuesta = PERFECTO;

		instanciaRamActual = list_get(datosMemoria->listaRAM,posicionDePagEnTLB);

		respuesta->informacion = instanciaRamActual->contenido;

	}

	else {

		posicionDePagEnRam = dondeEstaEnRam(instruccion.nroPagina,instruccion.pid);

		if (posicionDePagEnRam >= 0) {

			respuesta->respuesta = PERFECTO;

			instanciaRamActual = list_get(datosMemoria->listaRAM,posicionDePagEnTLB);

			respuesta->informacion = instanciaRamActual->contenido;

			//agregar a la tlb :'(
		}

		else{

			 posicionDePagEnRam = traerPaginaDesdeSwap(instruccion, respuesta);

		//agregar a la tlb :'(

		}
	}

	if (respuesta->respuesta == MANQUEADO)
		destruirProceso(instruccion.pid);

	enviarRespuesta(cpuATratar, *respuesta);

}

int dondeEstaEnRam(int nroPagina, int pid) {

	int var, posicionDePagina = -1;

	tipoRAM* estructuraRamActual;

	for (var = 0; var < list_size(datosMemoria->listaRAM); ++var) {

		estructuraRamActual = list_get(datosMemoria->listaRAM, var);

		if (estructuraRamActual->numeroDePagina == nroPagina&& estructuraRamActual->pid == pid) {

			posicionDePagina = var;

			break;
		}
	}

	return posicionDePagina;

}

int dondeEstaEnTLB(int nroPagina, int pid) {

	int var, posicionDePagina = -1;

	tipoTLB* estructuraTLBActual;

	for (var = 0; var < list_size(datosMemoria->listaTLB); ++var) {

		estructuraTLBActual = list_get(datosMemoria->listaRAM, var);

		if (estructuraTLBActual->numeroDePagina == nroPagina&& estructuraTLBActual->pid == pid) {

			posicionDePagina = estructuraTLBActual->posicionEnRAM;

			break;
		}
	}

	return posicionDePagina;
}

int traerPaginaDesdeSwap( tipoInstruccion instruccion, tipoRespuesta* respuesta) {

	//ejecutar algoritmos locos

	return PERFECTO;
}

void quitarPaginasDeTLB(int pid) {
	int var, posicionEnTLB;
	for (var = 0; var < cantidadDePaginasAsignadas(pid); ++var) {

		posicionEnTLB = dondeEstaEnTLB(var, pid);

		if (posicionEnTLB >= 0)
			list_remove(datosMemoria->listaTLB,posicionEnTLB);
	}

}
int cantidadDePaginasAsignadas(int pid){

	int var,cantidadDePaginasAsignadas = 0;

	tipoAdministracionPaginas* instanciaAdministracionActual;

	for (var = 0; var < list_size(datosMemoria->administradorPaginas); ++var) {

	instanciaAdministracionActual = list_get(datosMemoria->administradorPaginas,var);

	if(instanciaAdministracionActual->pid==pid){

		cantidadDePaginasAsignadas = instanciaAdministracionActual->paginasAsignadas;

		break;
		}
	}

	return cantidadDePaginasAsignadas;

}

void quitarPaginasDeRAM(int pid) {
	int var;

	for (var = 0; var < cantidadDePaginasAsignadas(pid); ++var) {

		liberarEstructuraRAM(dondeEstaEnRam(var, pid));
	}
}

void liberarEstructuraRAM(int posicionEnRam){

	tipoRAM* instanciaRamActual = list_get(datosMemoria->listaRAM,posicionEnRam);

	free(instanciaRamActual->contenido);

	list_remove(datosMemoria->listaRAM,posicionEnRam);
}

//////////////////
//ESCRIBIR PAGINA
/////////////////

void escribirPagina(tipoInstruccion instruccion,int cpuATratar){

	tipoRespuesta* respuesta;

if(strlen(instruccion.texto)<datosMemoria->configuracion->tamanioDeMarco){

		tipoRAM* instanciaRamActual;

		int posicionDePagEnRam = -1;

		int posicionDePagEnTLB = dondeEstaEnTLB(instruccion.nroPagina,instruccion.pid);

		if (posicionDePagEnTLB >= 0) {

			respuesta->respuesta = PERFECTO;

			instanciaRamActual = list_get(datosMemoria->listaRAM,posicionDePagEnTLB);

			free(instanciaRamActual->contenido);

			instanciaRamActual->contenido = malloc(datosMemoria->configuracion->tamanioDeMarco);

			instanciaRamActual->contenido = instruccion.texto;

			respuesta->informacion = instanciaRamActual->contenido;

			instanciaRamActual->modificado = true;
		}

		else {

			posicionDePagEnRam = dondeEstaEnRam(instruccion.nroPagina,instruccion.pid);

			if (posicionDePagEnRam >= 0) {

				respuesta->respuesta = PERFECTO;

				instanciaRamActual = list_get(datosMemoria->listaRAM,posicionDePagEnTLB);

				free(instanciaRamActual->contenido);

				instanciaRamActual->contenido = malloc(datosMemoria->configuracion->tamanioDeMarco);

				instanciaRamActual->contenido = instruccion.texto;

				respuesta->informacion = instanciaRamActual->contenido;

				instanciaRamActual->modificado = true;

				//agregar a la tlb :'(
			}

			else

			posicionDePagEnRam = traerPaginaDesdeSwap(instruccion, respuesta);

			instanciaRamActual = list_get(datosMemoria->listaRAM,posicionDePagEnRam);

			free(instanciaRamActual->contenido);

			instanciaRamActual->contenido = malloc(datosMemoria->configuracion->tamanioDeMarco);

			instanciaRamActual->contenido = instruccion.texto;

			respuesta->informacion = instanciaRamActual->contenido;

			instanciaRamActual->modificado = true;

			//agregar a la tlb :'(
		}

		if (respuesta->respuesta == MANQUEADO)
			destruirProceso(instruccion.pid);

}

else{

	respuesta->respuesta = MANQUEADO;

	respuesta->informacion = "";

	destruirProceso(instruccion.pid);
}

	enviarRespuesta(cpuATratar, *respuesta);
}

////////////////////
//FINALIZAR PROCESO
////////////////////

bool instruccionASwapRealizada(tipoInstruccion instruccion,tipoRespuesta* respuesta) {

	enviarInstruccion(datosMemoria->socketSWAP, instruccion);

	respuesta = recibirRespuesta(datosMemoria->socketSWAP);

	printf("recibi respuesta de swap\n");

	printf("el estado de respuesta es %c\n",respuesta->respuesta);

	if(respuesta->respuesta==NULL)
		printf("No se puede leer estado de respuesta\n");

	return (respuesta->respuesta == PERFECTO);
}

void quitarProceso(tipoInstruccion instruccion, int cpuaATratar) {

	tipoRespuesta* respuesta;

	instruccionASwapRealizada(instruccion, respuesta);

	if (respuesta->respuesta == PERFECTO) {

		destruirProceso(instruccion.pid);
	}

	enviarRespuesta(cpuaATratar, *respuesta);
}

void destruirProceso(int pid) {

	quitarPaginasDeRAM(pid);

	quitarPaginasDeTLB(pid);

	quitarAdministracionDePaginas(pid);
}

void quitarAdministracionDePaginas(int pid){

	int var;

	tipoAdministracionPaginas* instanciaAdministracionPagina;

	for (var = 0; var < list_size(datosMemoria->administradorPaginas); ++var) {

		instanciaAdministracionPagina = list_get(datosMemoria->administradorPaginas,var);

		if(instanciaAdministracionPagina->pid==pid){
			list_remove(datosMemoria->administradorPaginas,var);
			break;
		}
	}
}

