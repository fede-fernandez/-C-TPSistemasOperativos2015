/*
 * instruccionesSWAP.c
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */


#include "instruccionesSWAP.h"


tipoRespuesta* ejecutarInstruccion(tipoInstruccion* instruccion,t_list* listaDeHuecosUtilizados,t_list* listaDeHuecosLibres,tipoConfigSWAP* configuracion, t_log* logger, bool* finalizarProceso){

	tipoRespuesta* respuesta;

	switch (instruccion->instruccion) {
		case INICIAR:
			respuesta = reservarEspacio(listaDeHuecosUtilizados,listaDeHuecosLibres,instruccion->pid,instruccion->nroPagina,configuracion->cantidadDePaginas,configuracion->tamanioDePagina,configuracion->nombreDeSWAP,configuracion->retardoDeCompactacion, logger);
			break;

		case LEER:
			respuesta = leerPagina(listaDeHuecosUtilizados,instruccion->pid,instruccion->nroPagina,configuracion->tamanioDePagina,configuracion->nombreDeSWAP, logger,configuracion->retardoDeSWAP);
			break;

		case ESCRIBIR:
			respuesta = escribirPagina(listaDeHuecosUtilizados,instruccion->pid,instruccion->texto,instruccion->nroPagina,configuracion->tamanioDePagina,configuracion->nombreDeSWAP, logger,configuracion->retardoDeSWAP);
			break;

		case FINALIZAR:
			respuesta = liberarEspacio(listaDeHuecosUtilizados,listaDeHuecosLibres,instruccion->pid, configuracion->tamanioDePagina, logger,configuracion->nombreDeSWAP);
			break;

		case FINALIZAR_PROCESO:
			respuesta = finalizarProcesoSWAP(finalizarProceso);
			break;
	}

	return respuesta;

}



tipoRespuesta* reservarEspacio(t_list* listaDeHuecosUtilizados,t_list* listaDeHuecosLibres,int pidProcesoNuevo, int cantPaginasSolicitadas,int cantDePaginasDeSWAP,int tamanioDePagina, char* particion, int retardoDeCompactacion, t_log* logger){

	tipoRespuesta* respuestaASolicitudDeReserva;
	int baseParaNuevoPID;
	char* textoALogear = string_new();
	bool tengoEspacioContiguoParaAlmacenarProceso;


	if (cantidadDePaginasDisponibles(listaDeHuecosLibres) >= cantPaginasSolicitadas) {
		tengoEspacioContiguoParaAlmacenarProceso = tengoEspacioContiguo(listaDeHuecosLibres,cantPaginasSolicitadas,&baseParaNuevoPID);

		if(!tengoEspacioContiguoParaAlmacenarProceso){
			compactacionAlpha(listaDeHuecosUtilizados,listaDeHuecosLibres,particion,tamanioDePagina,cantDePaginasDeSWAP, retardoDeCompactacion, logger);

			baseParaNuevoPID = ultimaPaginaEscrita(listaDeHuecosUtilizados) + 1;
		}


		asignarEspacio(listaDeHuecosUtilizados,listaDeHuecosLibres,pidProcesoNuevo,baseParaNuevoPID,cantPaginasSolicitadas);

		respuestaASolicitudDeReserva = crearTipoRespuesta(PERFECTO,OK_ESPACIO_RESERVADO);

		string_append_with_format(&textoALogear,"Proceso mProc asignado  |  PID: %d  |  Byte inicial: %d  | Tamanio en bytes: %d", pidProcesoNuevo, baseParaNuevoPID*tamanioDePagina, cantPaginasSolicitadas*tamanioDePagina);
		logearSeguimiento(textoALogear,logger);
	}
	else {

		respuestaASolicitudDeReserva = crearTipoRespuesta(MANQUEADO,ERROR_NO_HAY_ESPACIO_EN_SWAP);

		string_append_with_format(&textoALogear,"Proceso mProc rechazado | Solicitud: %d paginas | Espacio disponible: %d paginas", cantPaginasSolicitadas,cantidadDePaginasDisponibles(listaDeHuecosLibres));
		logearSeguimiento(textoALogear,logger);

	}

	free(textoALogear);

	return respuestaASolicitudDeReserva;
}

tipoRespuesta* liberarEspacio(t_list* listaDeHuecosUtilizados,t_list* listaDeHuecosLibres,int pidProceso, int tamanioDePagina, t_log* logger, char* rutaDeParticion){

	tipoRespuesta* respuestaASolicitudDeLiberacion;
	char* textoALogear = string_new();
	int base, cantidadDePaginas;

	//buscarHuecoDePIDyBorrarHuecoDeLista
	liberarProcesoDeListaDeHuecosUtilizadosYDeParticion(listaDeHuecosUtilizados,rutaDeParticion,tamanioDePagina,pidProceso,&base,&cantidadDePaginas);

	//actualizarListaDeLibres
	actualizarListaDeHuecosLibres(listaDeHuecosLibres,base,cantidadDePaginas);


	respuestaASolicitudDeLiberacion = crearTipoRespuesta(PERFECTO,OK_ESPACIO_LIBERADO);

	string_append_with_format(&textoALogear,"Proceso mProc liberado  |  PID: %d  |  Byte inicial: %d  | Tamanio en bytes liberado: %d", pidProceso, base*tamanioDePagina, cantidadDePaginas*tamanioDePagina);
	logearSeguimiento(textoALogear,logger);
	free(textoALogear);

	return respuestaASolicitudDeLiberacion;
}

tipoRespuesta* leerPagina(t_list* listaDeHuecosUtilizados,int pidProceso,int dirLogicaDePagina,int tamanioDePagina,char* particion, t_log* logger, int retardoDeLectura){
	tipoRespuesta* respuestaASolicitudDeLectura;

	char* contenidoDePagina;
	char* textoALogear = string_new();

	//buscarPIDEnListaDeHuecos
	tipoHuecoUtilizado* huecoDelProceso = buscarHuecoUtilizadoPorPID(listaDeHuecosUtilizados,pidProceso);

	//traducirDireccionLogicaEnFisica
	int direccionFisicaEnParticion = traducirDireccionLogicaAFisica(huecoDelProceso,dirLogicaDePagina);
		//direccionFisicaEnParticion es la posicion donde comienza la pagina en el archivo de particion

	//buscarEnContenidoEnParticion
	contenidoDePagina = string_duplicate(leerBloqueMapeado(particion,direccionFisicaEnParticion,tamanioDePagina));

	//retornarContenidoDePagina

	respuestaASolicitudDeLectura = crearTipoRespuesta(PERFECTO,contenidoDePagina);

	sleep(retardoDeLectura);

	string_append_with_format(&textoALogear,"Lectura realizada  |  PID: %d  |  Byte inicial: %d  | Tamanio: %d  |  Contenido: %s",pidProceso,dirLogicaDePagina*tamanioDePagina,string_length(contenidoDePagina),contenidoDePagina);
	logearSeguimiento(textoALogear,logger);

	free(textoALogear);
	free(contenidoDePagina);

	return respuestaASolicitudDeLectura;
}

tipoRespuesta* escribirPagina(t_list* listaDeHuecosUtilizados,int pidProceso,char* contenidoAEscribir,int dirLogicaDePagina,int tamanioDePagina, char* particion, t_log* logger, int retardoDeEscritura){
	tipoRespuesta* respuestaASolicitudDeEscritura;
	char* textoALogear = string_new();

	//buscarPIDEnListaDeHuecos
	tipoHuecoUtilizado* huecoDelProceso = buscarHuecoUtilizadoPorPID(listaDeHuecosUtilizados,pidProceso);

	//traducirDireccionLogicaAFisica
	int direccionFisicaEnParticion = traducirDireccionLogicaAFisica(huecoDelProceso,dirLogicaDePagina);

	//escribirEnParticion
	escribirBloqueMapeado(particion,contenidoAEscribir,direccionFisicaEnParticion,tamanioDePagina);


	respuestaASolicitudDeEscritura = crearTipoRespuesta(PERFECTO,OK_PAGINA_ESCRITA);

	sleep(retardoDeEscritura);

	string_append_with_format(&textoALogear,"Escritura realizada  |  PID: %d  |  Byte inicial: %d  | Tamanio: %d  |  Contenido: %s",pidProceso,dirLogicaDePagina*tamanioDePagina,string_length(contenidoAEscribir),contenidoAEscribir);
	logearSeguimiento(textoALogear,logger);

	free(textoALogear);

	return respuestaASolicitudDeEscritura;
}

tipoRespuesta* finalizarProcesoSWAP(bool* finalizarProceso){
	*finalizarProceso = true;
	return crearTipoRespuesta(PERFECTO,"");
}
