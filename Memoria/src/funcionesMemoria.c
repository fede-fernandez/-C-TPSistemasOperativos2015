#include "funcionesMemoria.h"
#include <commonsDeAsedio/estructuras.h>
#include <commons/string.h>

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

	datosMemoria->listaTablaPaginas = list_create();

	datosMemoria->listaRAM = list_create();

	datosMemoria->listaAccesosAPaginasRAM = list_create();

	if(estaHabilitadaLaTLB()){
	datosMemoria->listaTLB = list_create();
	datosMemoria->listaAccesosAPaginasTLB = list_create();
	}
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

	tipoRespuesta* respuesta;// = malloc(sizeof(tipoRespuesta));

	if (puedoReservarEnSWAP(instruccion, &respuesta)) {

		printf("pude reservar en swap!!\n");

		tipoTablaPaginas* tablaDePaginasNueva = malloc(sizeof(tipoTablaPaginas));

		printf("pedi instancia de admin\n");

		tablaDePaginasNueva->frames = list_create();

		tablaDePaginasNueva->paginasAsignadas = 0;

		tablaDePaginasNueva->paginasPedidas = instruccion.nroPagina;

		tablaDePaginasNueva->pid = instruccion.pid;

		printf("cree instancia de administracion..\n");

		list_add(datosMemoria->listaTablaPaginas, tablaDePaginasNueva);

		printf("agregue pagina a tabla de paginas\n");
	}

	enviarRespuesta(cpuATratar, respuesta);
}

bool puedoReservarEnSWAP(tipoInstruccion instruccion, tipoRespuesta* respuesta) {

	return instruccionASwapRealizada(&instruccion, respuesta);
}

//////////////////
//LEER PAGINA
/////////////////

void enviarPaginaPedidaACpu(tipoInstruccion instruccion, int cpuATratar) {

	tipoRespuesta* respuesta = malloc(sizeof(tipoRespuesta));

	int posicionDePag = -1,dondeEstaTabla = buscarTabla(instruccion.pid);

	bool estaEnTLB = true;

	if(dondeEstaTabla>=0){

		if(estaHabilitadaLaTLB())
			posicionDePag = dondeEstaEnTLB(instruccion.nroPagina,instruccion.pid);


		if (posicionDePag<0) {

		posicionDePag = dondeEstaEnTabla(instruccion.nroPagina,instruccion.pid);

		estaEnTLB = false;
		}

		if(posicionDePag<0) {

		 traerPaginaDesdeSwap(instruccion, respuesta);

		 posicionDePag = dondeEstaEnTabla(instruccion.nroPagina,instruccion.pid);
		}


		if(posicionDePag>=0){

		respuesta->respuesta = PERFECTO;

		respuesta->informacion = malloc(datosMemoria->configuracion->tamanioDeMarco);

		respuesta->informacion = traerPaginaDesdeRam(posicionDePag);

		if(!estaEnTLB&&estaHabilitadaLaTLB())
			agregarPaginaATLB(instruccion.nroPagina,instruccion.pid,posicionDePag);

				}
		else{
			tipoInstruccion instruccionDeBorrado;

			instruccionDeBorrado.instruccion = FINALIZAR;

			instruccionDeBorrado.pid = instruccion.pid;

			instruccionDeBorrado.nroPagina = 0;

			instruccionDeBorrado.texto = "";

			tipoRespuesta* respuestaSwap;

			if(instruccionASwapRealizada(&instruccionDeBorrado,respuestaSwap))
				destruirProceso(instruccion.pid);

			respuesta->respuesta = MANQUEADO;

			respuesta->informacion = "Pagina no encontrada";
		}
	}

	else{
					tipoInstruccion instruccionDeBorrado;

					instruccionDeBorrado.instruccion = FINALIZAR;

					instruccionDeBorrado.pid = instruccion.pid;

					instruccionDeBorrado.nroPagina = 0;

					instruccionDeBorrado.texto = "";

					tipoRespuesta* respuestaSwap;

					if(instruccionASwapRealizada(&instruccionDeBorrado,respuestaSwap))
					destruirProceso(instruccion.pid);

					respuesta->respuesta = MANQUEADO;

					respuesta->informacion = "Tabla de paginas no existente";
				}

	enviarRespuesta(cpuATratar, respuesta);

	}

char* traerPaginaDesdeRam(int direccion){

	sleep(datosMemoria->configuracion->retardoDeMemoria);

	char* pagina = list_get(datosMemoria->listaRAM,direccion);

	return pagina;

}

int buscarTabla(int pid){

int var,posicionDeTabla = -1;

tipoTablaPaginas* tablaActual;

for (var = 0; var < list_size(datosMemoria->listaTablaPaginas); ++var) {

	tablaActual = list_get(datosMemoria->listaTablaPaginas,var);

	if(tablaActual->pid==pid){

		posicionDeTabla = var;

		break;
	}
}

return posicionDeTabla;

}

int dondeEstaEnTabla(int nroPagina, int pid) {

	sleep(datosMemoria->configuracion->retardoDeMemoria);

	int posicionDePagina = -1,posicionDeTabla = buscarTabla(pid);

	tipoTablaPaginas* tablaActual;

	tipoPagina* paginaActual;

	if(posicionDeTabla>=0){

	tablaActual = list_get(datosMemoria->listaTablaPaginas, posicionDeTabla);

		int i;
		for (i = 0; i < list_size(tablaActual->frames); ++i) {

			paginaActual = list_get(tablaActual->frames,i);

			if(paginaActual->numeroDePagina==nroPagina){

				posicionDePagina = paginaActual->posicionEnRAM;

				break;
			}

		}

		}
	if(posicionDePagina>=0)
		aumentarAccesoAPaginaRAM(nroPagina,pid);

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

	if(posicionDePagina>=0)
		aumentarAccesoAPaginaTLB(nroPagina,pid);

	return posicionDePagina;
}

void traerPaginaDesdeSwap(tipoInstruccion instruccion, tipoRespuesta* respuesta) {

	//ejecutar algoritmos locos

}

void aumentarAcceso(int nroPagina,int pid,t_list* listaAccesos){

	tipoAccesosAPagina* accesoActual;

	int var;
	for (var = 0; var < list_size(listaAccesos); ++var) {

		accesoActual = list_get(listaAccesos,var);

		if(accesoActual->pid==pid&&accesoActual->nroPagina==nroPagina){

			accesoActual->cantVecesAccedido++;

			break;
		}
	}
}

void aumentarAccesoAPaginaTLB(int nroPagina,int pid){

	if(datosMemoria->tipoDeAlgoritmoTLB==LRU)
		aumentarAcceso(nroPagina,pid,datosMemoria->listaAccesosAPaginasTLB);
}

void aumentarAccesoAPaginaRAM(int nroPagina,int pid){

	if(datosMemoria->tipoDeAlgoritmoRAM==LRU)
		aumentarAcceso(nroPagina,pid,datosMemoria->listaAccesosAPaginasRAM);
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

	tipoTablaPaginas* tablaActual;
	/*

	tipoAdministracionPaginas* instanciaAdministracionActual;

	for (var = 0; var < list_size(datosMemoria->administradorPaginas); ++var) {

	instanciaAdministracionActual = list_get(datosMemoria->administradorPaginas,var);

	if(instanciaAdministracionActual->pid==pid){

		cantidadDePaginasAsignadas = instanciaAdministracionActual->paginasAsignadas;

		break;
		}
	}*/
	for (var = 0; var < list_size(datosMemoria->listaTablaPaginas); ++var) {

		tablaActual = list_get(datosMemoria->listaTablaPaginas,var);

		if(tablaActual->pid==pid){

			cantidadDePaginasAsignadas = tablaActual->paginasAsignadas;
		}

	}

	return cantidadDePaginasAsignadas;

}

void quitarTabla(int pid) {

	int var,dondeEstaTabla = buscarTabla(pid);

	tipoTablaPaginas* tablaDeProcesoABorrar = list_get(datosMemoria->listaTablaPaginas,dondeEstaTabla);

	tipoPagina* paginaActual;

	for (var = 0; var < cantidadDePaginasAsignadas(pid); ++var) {

		paginaActual = list_get(tablaDeProcesoABorrar->frames,var);

		liberarPaginaDeRAM(dondeEstaEnTabla(paginaActual->numeroDePagina,pid));

		free(paginaActual);
	}

	free(tablaDeProcesoABorrar);

	list_remove(datosMemoria->listaTablaPaginas,dondeEstaTabla);
}

void liberarPaginaDeRAM(int posicionEnRam){

	char* pagina = list_get(datosMemoria->listaRAM,posicionEnRam);

	free(pagina);

	list_remove(datosMemoria->listaRAM,posicionEnRam);
}

//////////////////
//ESCRIBIR PAGINA
/////////////////

bool estaHabilitadaLaTLB(){

	return string_equals_ignore_case(datosMemoria->configuracion->TLBHabilitada,"SI");
}

void escribirPagina(tipoInstruccion instruccion,int cpuATratar){

	tipoRespuesta* respuesta = malloc(sizeof(tipoRespuesta));

	int posicionDePag = -1,dondeEstaTabla = buscarTabla(instruccion.pid);

	bool estaEnTLB = true;

	if(dondeEstaTabla>=0){

		tipoTablaPaginas* tablaDeProceso = list_get(datosMemoria->listaTablaPaginas,dondeEstaTabla);

	if(strlen(instruccion.texto)<datosMemoria->configuracion->tamanioDeMarco&&instruccion.nroPagina<=tablaDeProceso->paginasPedidas){

		if(estaHabilitadaLaTLB())
			posicionDePag = dondeEstaEnTLB(instruccion.nroPagina,instruccion.pid);


		if (posicionDePag<0) {

		posicionDePag = dondeEstaEnTabla(instruccion.nroPagina,instruccion.pid);

		estaEnTLB = false;
		}

		if(posicionDePag<0) {

		 traerPaginaDesdeSwap(instruccion, respuesta);

		 posicionDePag = dondeEstaEnTabla(instruccion.nroPagina,instruccion.pid);
		}

		respuesta->respuesta = PERFECTO;

		respuesta->informacion = malloc(datosMemoria->configuracion->tamanioDeMarco);

		memcpy(respuesta->informacion,instruccion.texto,datosMemoria->configuracion->tamanioDeMarco);

		if(posicionDePag>=0){

		char* paginaAModificar = traerPaginaDesdeRam(posicionDePag);

		free(paginaAModificar);

		paginaAModificar = malloc(datosMemoria->configuracion->tamanioDeMarco);

		memcpy(paginaAModificar,instruccion.texto,datosMemoria->configuracion->tamanioDeMarco);

		list_replace(datosMemoria->listaRAM,posicionDePag,paginaAModificar);

		modificarBitDeModificacion(instruccion.nroPagina,instruccion.pid);

		if(!estaEnTLB&&estaHabilitadaLaTLB())
			agregarPaginaATLB(instruccion.nroPagina,instruccion.pid,posicionDePag);

				}
		else
			agregarPagina(instruccion.nroPagina,instruccion.pid,instruccion.texto);
		}

	else{

		tipoInstruccion instruccionDeBorrado;

		instruccionDeBorrado.instruccion = FINALIZAR;//Charlar conForronan porque es un error

		instruccionDeBorrado.pid = instruccion.pid;

		instruccionDeBorrado.nroPagina = 0;

		instruccionDeBorrado.texto = "";

		tipoRespuesta* respuestaSwap;

		if(instruccionASwapRealizada(&instruccionDeBorrado,respuestaSwap))
		destruirProceso(instruccion.pid);

		respuesta->respuesta = MANQUEADO;

		if(instruccion.nroPagina>tablaDeProceso->paginasPedidas)
		respuesta->informacion = "Tamaño de pagina demasiado grande";

		else
			respuesta->informacion = "Numero de pagina demasiado grande";

		}
	}

				else{

					tipoInstruccion instruccionDeBorrado;

					instruccionDeBorrado.instruccion = FINALIZAR;

					instruccionDeBorrado.pid = instruccion.pid;

					instruccionDeBorrado.nroPagina = 0;

					instruccionDeBorrado.texto = "";

					tipoRespuesta* respuestaSwap;

					if(instruccionASwapRealizada(&instruccionDeBorrado,respuestaSwap))
					destruirProceso(instruccion.pid);

					respuesta->respuesta = MANQUEADO;

					respuesta->informacion = "Tabla de paginas no existente";
				}

	enviarRespuesta(cpuATratar, respuesta);
	}

void modificarBitDeModificacion(int nroPagina,int pid){

	int dondeEstaTabla = buscarTabla(pid),var;

	tipoTablaPaginas* instanciaTabla = list_get(datosMemoria->listaTablaPaginas,dondeEstaTabla);

	tipoPagina* paginaActual;

	for (var = 0; var < list_size(instanciaTabla->frames); ++var) {

		paginaActual = list_get(instanciaTabla->frames,var);

		if(paginaActual->numeroDePagina==nroPagina){

			paginaActual->modificado = !paginaActual->modificado;

			break;
		}

	}

}


////////////////////
//FINALIZAR PROCESO
////////////////////

bool instruccionASwapRealizada(tipoInstruccion* instruccion,tipoRespuesta** respuesta) {

	enviarInstruccion(datosMemoria->socketSWAP, instruccion);

	*respuesta = recibirRespuesta(datosMemoria->socketSWAP);

	printf("recibi respuesta de swap\n");

	printf("el estado de respuesta es %c\n",(*respuesta)->respuesta);

	printf("La info de respuesta es: %s\n",(*respuesta)->informacion);

	if((*respuesta)->respuesta==NULL)
		printf("No se puede leer estado de respuesta\n");

	return ((*respuesta)->respuesta == PERFECTO);
}

void quitarProceso(tipoInstruccion instruccion, int cpuaATratar) {

	tipoRespuesta* respuesta;

	instruccionASwapRealizada(&instruccion, respuesta);

	if (respuesta->respuesta == PERFECTO) {

		destruirProceso(instruccion.pid);
	}

	enviarRespuesta(cpuaATratar, respuesta);
}

void destruirProceso(int pid) {

	quitarTabla(pid);

	quitarPaginasDeTLB(pid);

	//quitarAdministracionDePaginas(pid);
}


/*void quitarAdministracionDePaginas(int pid){

	int var;

	tipoAdministracionPaginas* instanciaAdministracionPagina;

	for (var = 0; var < list_size(datosMemoria->administradorPaginas); ++var) {

		instanciaAdministracionPagina = list_get(datosMemoria->administradorPaginas,var);

		if(instanciaAdministracionPagina->pid==pid){
			list_remove(datosMemoria->administradorPaginas,var);
			break;
		}
	}
}*/

//**************************************************************************************************************
//**********************************FUNCIONES DE REEMPLAZO******************************************************
//**************************************************************************************************************

void agregarPaginaATLB(int nroPagina,int pid,int posicionEnRam){

	if(!TLBLlena()){

		tipoTLB* instanciaTLB = malloc(sizeof(tipoTLB));

		instanciaTLB->numeroDePagina = nroPagina;

		instanciaTLB->pid = pid;

		instanciaTLB->posicionEnRAM = posicionEnRam;

		list_add(datosMemoria->listaTLB,instanciaTLB);
	}

	else{

		int posicionAReemplazar = cualReemplazarTLB();

		list_remove(datosMemoria->listaAccesosAPaginasTLB,posicionAReemplazar);

		tipoTLB* instanciaTLB = malloc(sizeof(tipoTLB));

		instanciaTLB->numeroDePagina = nroPagina;

		instanciaTLB->pid = pid;

		instanciaTLB->posicionEnRAM = posicionEnRam;

	}

}

void agregarPaginaATabla(int nroPagina,int pid,int posicionEnRam){

	int dondeEstaTabla = buscarTabla(pid);

	tipoPagina* instanciaPagina = malloc(sizeof(tipoPagina));

	instanciaPagina->modificado = false;

	instanciaPagina->numeroDePagina =  nroPagina;

	instanciaPagina->posicionEnRAM = posicionEnRam;
}

bool agregarPagina(int nroPagina,int pid,char* pagina){

	bool operacionExitosa = true;

	int posicionEnRam;

	if(RAMLlena()){

			int posicionAReemplazar = cualReemplazarRAM();

			tipoAccesosAPagina* instanciaAccesoAReemplzar = list_get(datosMemoria->listaAccesosAPaginasRAM,posicionAReemplazar);

			tipoInstruccion* instruccion = malloc(sizeof(tipoInstruccion));

			instruccion->nroPagina = instanciaAccesoAReemplzar->nroPagina;

			instruccion->pid = instanciaAccesoAReemplzar->pid;

			instruccion->instruccion = ESCRIBIR;//Esto hay que charlarlo con Forronan

			instruccion->texto = malloc(datosMemoria->configuracion->tamanioDeMarco);

			tipoRespuesta* respuestaSwap;

			operacionExitosa = instruccionASwapRealizada(instruccion,respuestaSwap);

			if(operacionExitosa){

				posicionEnRam = dondeEstaEnTLB(instanciaAccesoAReemplzar->nroPagina,instanciaAccesoAReemplzar->pid);

				if(posicionEnRam>=0)
					quitarPaginaDeTLB(instanciaAccesoAReemplzar->nroPagina,instanciaAccesoAReemplzar->pid);

				else{
					posicionEnRam = dondeEstaEnTabla(instanciaAccesoAReemplzar->nroPagina,instanciaAccesoAReemplzar->pid);

					quitarPaginaDeTabla(instanciaAccesoAReemplzar->nroPagina,instanciaAccesoAReemplzar->pid);
				}

				list_remove(datosMemoria->listaRAM,posicionEnRam);
			}
	}
	else
		posicionEnRam = list_size(datosMemoria->listaRAM);


	if(operacionExitosa){

				list_add(datosMemoria->listaRAM,pagina);

				agregarPaginaATabla(nroPagina,pid,posicionEnRam);

				if(estaHabilitadaLaTLB())
				agregarPaginaATLB(nroPagina,pid,posicionEnRam);
			}

	return operacionExitosa;
}

void quitarPaginaDeTLB(int nroPagina,int pid){

	int var;

	tipoTLB* instanciaTLB;

	for (var = 0; var < list_size(datosMemoria->listaTLB); ++var) {

		instanciaTLB= list_get(datosMemoria->listaTLB,var);

		if(instanciaTLB->numeroDePagina==nroPagina&&instanciaTLB->pid==pid){

			list_remove(datosMemoria->listaTLB,var);

			break;
		}
	}

}

void quitarPaginaDeTabla(int nroPagina,int pid){

	int var;

	tipoTablaPaginas* instanciaTabla = buscarTabla(pid);

	tipoPagina* instanciaPagina;

	for (var = 0; var < list_size(instanciaTabla->frames); ++var) {

		instanciaPagina = list_get(instanciaTabla->frames,var);

		if(instanciaPagina->numeroDePagina==nroPagina){

			list_remove(instanciaTabla->frames,var);

			break;
		}
	}

}


int cualReemplazarRAM(){

	int cualReemplazar;

	switch (datosMemoria->tipoDeAlgoritmoRAM) {

	case FIFO:
		cualReemplazar = cualReemplazarRAMFIFO();
			break;

	case LRU:
		cualReemplazar = cualReemplazarRAMLRU();
			break;
	}

	return cualReemplazar;
}

bool RAMLlena(){

	return (list_size(datosMemoria->listaRAM)>=datosMemoria->configuracion->cantidadDeMarcos);
}

bool TLBLlena(){

	return list_size(datosMemoria->listaTLB)>=datosMemoria->configuracion->entradasDeTLB;
}

int cualReemplazarRAMFIFO(){

	return cualReemplazarPorFIFO(datosMemoria->listaAccesosAPaginasRAM);

}

int cualReemplazarRAMLRU(){

	return cualReemplazarPorLRU(datosMemoria->listaAccesosAPaginasRAM);

}

int cualReemplazarTLBFIFO(){

	return cualReemplazarPorFIFO(datosMemoria->listaAccesosAPaginasTLB);
}

int cualReemplazarTLBLRU(){

	return cualReemplazarPorLRU(datosMemoria->listaAccesosAPaginasTLB);
}

int cualReemplazarTLB(){

	int cualReemplazar;

switch (datosMemoria->tipoDeAlgoritmoTLB) {

	case FIFO:
		cualReemplazar = cualReemplazarTLBFIFO();
		break;

	case LRU:
		cualReemplazar = cualReemplazarTLBLRU();
		break;
}

	return cualReemplazar;
}

int cualReemplazarPorFIFO(t_list* listaAEvaluar){

	return 0;
}

int cualReemplazarPorLRU(t_list* listaAEvaluar){

	int var;

	tipoAccesosAPagina* instanciaAccesoActual;

	tipoAccesosAPagina* instanciaAccesoLRU = list_get(listaAEvaluar,0);

	int posicionLRU = 0;

	for (var = 1; var < list_size(listaAEvaluar); ++var) {

		instanciaAccesoActual = list_get(listaAEvaluar,var);

		if(instanciaAccesoActual->cantVecesAccedido<instanciaAccesoLRU->cantVecesAccedido){

			instanciaAccesoLRU = instanciaAccesoActual;

			posicionLRU = var;
		}
	}

	return posicionLRU;
}



