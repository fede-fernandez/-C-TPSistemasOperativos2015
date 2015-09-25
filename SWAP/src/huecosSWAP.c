/*
 * huecosSWAP.c
 *
 *  Created on: 13/9/2015
 *      Author: utnso
 */


#include "estructurasSWAP.h"
#include "huecosSWAP.h"
#include <commons/collections/list.h>

///////////////FUNCIONES PRIVADAS//////////////
int paginasLibresIntermedias(int cantDePaginasSolicitadas,	t_list* listaDeHuecosUtilizados) {
	int i;
	tipoHuecoUtilizado* h1;
	tipoHuecoUtilizado* h2;


	for (i = 0; i < list_size(listaDeHuecosUtilizados) - 1; ++i) {
		h1 = list_get(listaDeHuecosUtilizados, i);
		h2 = list_get(listaDeHuecosUtilizados, i + 1);
		if (espacioEntreDosHuecosUtilizados(h1, h2)	>= cantDePaginasSolicitadas) {
			return traducirDireccionLogicaAFisica(h1,h1->cantidadDePaginasQueOcupa);
		}
	}

	return -1;
}

int espacioEntreDosHuecosUtilizados(tipoHuecoUtilizado* h1, tipoHuecoUtilizado* h2){
	return (h2->baseDeMProc - (h1->baseDeMProc + h1->cantidadDePaginasQueOcupa));
}

int paginaMaxima(tipoHuecoUtilizado* hueco){
	return (hueco->baseDeMProc + hueco->cantidadDePaginasQueOcupa);
}

tipoHuecoUtilizado* buscarHuecoPorPID(t_list* listaDeHuecosUtilizados,int pidProcesoBuscado){
	tipoHuecoUtilizado* aux;
	int i;

	for (i = 0; i < list_size(listaDeHuecosUtilizados); ++i) {
		aux = list_get(listaDeHuecosUtilizados,i);
		if(aux->pid == pidProcesoBuscado)
			break;
	}
	return aux;
}

int traducirDireccionLogicaAFisica(tipoHuecoUtilizado* hueco,int dirLogicaDePagina){
	int direccionFisica;

	direccionFisica = hueco->baseDeMProc + dirLogicaDePagina;

	return direccionFisica;
}

void imprimirListaDeHuecos(t_list* lista){
	int i;

	for (i = 0; i < list_size(lista); ++i) {
		imprimirHueco(list_get(lista,i));
	}
}

void imprimirHueco(tipoHuecoUtilizado* hueco){

	printf("PID: %d   base: %d cantidadDePaginas: %d\n",hueco->pid,hueco->baseDeMProc,hueco->cantidadDePaginasQueOcupa);

}

int paginasLibresAlInicio(t_list* listaDeHuecosUtilizados){
	tipoHuecoUtilizado* aux = list_get(listaDeHuecosUtilizados,0);

	return aux->baseDeMProc;//Si el primer proceso en lista empieza, por ejemplo en la pagina 2, entonces significa que tiene 2 paginas libres {0,1}
}

int paginasLibresAlFinal(t_list* listaDeHuecosUtilizados, int cantTotalDePaginas){
	tipoHuecoUtilizado* aux = list_get(listaDeHuecosUtilizados,list_size(listaDeHuecosUtilizados)-1);

	int ultimaPaginaDeHueco = aux->baseDeMProc + aux->cantidadDePaginasQueOcupa;

	return cantTotalDePaginas - ultimaPaginaDeHueco;
}

int cantidadDePaginasOcupadas(t_list* listaDeHuecosOcupados){
	int paginasOcupadas = 0;
	tipoHuecoUtilizado* huecoAux;
	int i;

	if (!list_is_empty(listaDeHuecosOcupados)) {
		for (i = 0; i < list_size(listaDeHuecosOcupados); ++i) {
			huecoAux = list_get(listaDeHuecosOcupados,i);
			paginasOcupadas += huecoAux->cantidadDePaginasQueOcupa;
		}
	}

	return paginasOcupadas;
}

int indiceDeHuecoAnterior(t_list* listaDeHuecosUtilizados, int baseNuevo){
	tipoHuecoUtilizado* aux;
	int i;

	for (i = 0; i < list_size(listaDeHuecosUtilizados); ++i) {
		aux = list_get(listaDeHuecosUtilizados,i);
		if (baseNuevo == traducirDireccionLogicaAFisica(aux,aux->cantidadDePaginasQueOcupa)) {
			return i;
		}
		else {
			break;
		}
	}

	return -1;

}

bool baseMenor(tipoHuecoUtilizado* h1, tipoHuecoUtilizado* h2){
	return h1->baseDeMProc < h2->baseDeMProc;
}

/////////////////////LISTAS DE HUECOS///////////////////////


tipoHuecoUtilizado* crearHuecoUtilizado(int pidProceso,int inicio,int cantidadDePaginas){

	tipoHuecoUtilizado* aux = malloc(sizeof(tipoHuecoUtilizado));
	aux->pid = pidProceso;
	aux->baseDeMProc = inicio;
	aux->cantidadDePaginasQueOcupa = cantidadDePaginas;

	return aux;
}

void destruirHuecoUtilizado(tipoHuecoUtilizado* huecoUtilizado){
	free(huecoUtilizado);
}

t_list* inicializarListaDeHuecosUtilizados(){

	t_list*	listaDeHuecosUtilizados = list_create();
	return listaDeHuecosUtilizados;
}


/////////////////////FUNCIONES PRINCIPALES//////////////
int reservarEspacio(t_list* listaDeHuecosUtilizados,int pidProcesoNuevo, int cantPaginasSolicitadas,int cantDePaginasDeSWAP){

	int baseParaNuevoPID;

	//puedo reservar la cantidad de paginas que el mproc necesita?
	if (cantidadDePaginasDisponibles(listaDeHuecosUtilizados,cantDePaginasDeSWAP) >= cantPaginasSolicitadas) {
		//tengo espacio contiguo para almacenar las paginas que el mprc necesita?
		if((baseParaNuevoPID = baseParaMProcSiTengoEspacioContiguo(listaDeHuecosUtilizados,cantPaginasSolicitadas,cantDePaginasDeSWAP)) != -1){
			//compactar()
			//baseNuevo = ultima pagina ocupada para asignar al final
		}
		asignarEspacio(listaDeHuecosUtilizados,pidProcesoNuevo,cantPaginasSolicitadas,baseParaNuevoPID);

	}
	else {
		//enviar error a memoria
		printf("Error al asignar: no hay espacio suficiente");
	}

	return 0;
}

void liberarEspacio(t_list* listaDeHuecosUtilizados,int pidProceso){
	//buscarHuecoDePIDyBorrarHuecoDeLista
	tipoHuecoUtilizado* aux;
	int i;

	for (i = 0; i < list_size(listaDeHuecosUtilizados); ++i) {
		aux = list_get(listaDeHuecosUtilizados,i);
		if (aux->pid == pidProceso) {
			list_remove(listaDeHuecosUtilizados,i);
		}
	}

}

char* leerPagina(t_list* listaDeHuecosUtilizados,int pidProceso,int dirLogicaDePagina,int tamanioDePagina,FILE* particion){
	char* contenidoDePagina;

	//buscarPIDEnListaDeHuecos
	tipoHuecoUtilizado* huecoDelProceso = buscarHuecoPorPID(listaDeHuecosUtilizados,pidProceso);

	//traducirDireccionLogicaEnFisica
	int direccionFisicaEnParticion = traducirDireccionLogicaAFisica(huecoDelProceso,dirLogicaDePagina);
		//direccionFisicaEnParticion es la posicion donde comienza la pagina en el archivo de particion

	//buscarEnContenidoEnParticion
	contenidoDePagina = string_duplicate(leerBloqueMapeado(particion,direccionFisicaEnParticion,tamanioDePagina));

	//retornarContenidoDePagina
	return contenidoDePagina;
}

void escribirPagina(t_list* listaDeHuecosUtilizados,int pidProceso,char* contenidoAEscribir,int dirLogicaDePagina,int tamanioDePagina, FILE* particion){
	//buscarPIDEnListaDeHuecos
	tipoHuecoUtilizado* huecoDelProceso = buscarHuecoPorPID(listaDeHuecosUtilizados,pidProceso);

	//traducirDireccionLogicaAFisica
	int direccionFisicaEnParticion = traducirDireccionLogicaAFisica(huecoDelProceso,dirLogicaDePagina);

	//escribirEnParticion
	escribirBloqueMapeado(particion,contenidoAEscribir,direccionFisicaEnParticion,tamanioDePagina);
}


/////////////////FUNCIONES SECUNDARIAS//////////////

int cantidadDePaginasDisponibles(t_list* listaDeHuecosUtilizados,int cantTotalDePaginas){
	int paginasLibres;
	int paginasOcupadas;

	paginasOcupadas = cantidadDePaginasOcupadas(listaDeHuecosUtilizados);

	paginasLibres = cantTotalDePaginas - paginasOcupadas;

	return paginasLibres;
}


int baseParaMProcSiTengoEspacioContiguo(t_list* listaDeHuecosUtilizados, int cantDePaginasSolicitadas, int cantTotalDePaginas){

	int pagina;

	tipoHuecoUtilizado* h1;
	tipoHuecoUtilizado* h2;
	int i;

	if (paginasLibresAlInicio(listaDeHuecosUtilizados) >= cantDePaginasSolicitadas) {
		return 0;
	}
	//Verifico espacio contiguo entre bloques
	if ((pagina = paginasLibresIntermedias(cantDePaginasSolicitadas,listaDeHuecosUtilizados)) != -1) {
		return pagina;
	}

	if ((pagina = paginasLibresAlFinal(listaDeHuecosUtilizados,cantTotalDePaginas)) >= cantDePaginasSolicitadas) {
		return cantTotalDePaginas - pagina;
	}

	return -1;
}

 void compactar(){
}

void asignarEspacio(t_list* listaDeHuecosUtilizados,int pidProceso,int cantDePaginasSolicitadas,int base){
	//ARREGLAR
//	tipoHuecoUtilizado* aux;
	tipoHuecoUtilizado* huecoNuevo = crearHuecoUtilizado(pidProceso,base,cantDePaginasSolicitadas);
//	int indiceHuecoPrevio = indiceDeHuecoAnterior(listaDeHuecosUtilizados,base);
//
//	//agregarAListaOcupados
//	list_add_in_index(listaDeHuecosUtilizados,indiceHuecoPrevio+1,huecoNuevo);
	list_add(listaDeHuecosUtilizados,huecoNuevo);
	list_sort(listaDeHuecosUtilizados,(void*)baseMenor);
}

int buscarBaseDeHuecoVacioMasProximo(t_list* listaDeHuecosUtilizados, int baseAnterior){
	//si baseAnterior es distinto de base de hueco mas proximo
	// mover en archivo
	// mover en lista

	return 0;
}

int baseDeHuecoMasProximoA(int baseAnterior){



	return 0;
}

void compactacionAlpha(t_list* listaDeHuecosUtilizados, FILE* particion,int tamanioDePagina){
	tipoHuecoUtilizado* hueco;
	int ultimaPaginaEscrita = 0;


	int i;
	for (i = 0; i < list_size(listaDeHuecosUtilizados); ++i) {
		hueco = list_get(listaDeHuecosUtilizados,i);

		moverHueco(hueco,particion,ultimaPaginaEscrita,tamanioDePagina);
		ultimaPaginaEscrita = traducirDireccionLogicaAFisica(hueco,hueco->cantidadDePaginasQueOcupa);
	}
}

void moverHueco(tipoHuecoUtilizado* hueco,FILE* particion, int ultimaPaginaEscrita,int tamanioDePagina){
	int direccionFisicaBase = traducirDireccionLogicaAFisica(hueco,hueco->baseDeMProc);
	hueco->baseDeMProc = ultimaPaginaEscrita;

	int dirFisicaVieja;
	int dirFisicaNueva;

	int i;
	for (i = 0; i < hueco->cantidadDePaginasQueOcupa; ++i) {


		dirFisicaVieja = direccionFisicaBase + i;
		dirFisicaNueva = traducirDireccionLogicaAFisica(hueco,i);

		moverPagina(particion,dirFisicaVieja,dirFisicaNueva,tamanioDePagina);
	}

	hueco->baseDeMProc = ultimaPaginaEscrita;


}

void moverPagina(FILE* particion, int dirFisVieja, int dirFisNueva,int tamanioDePagina){

	char* pagina = leerBloqueMapeado(particion,dirFisVieja,tamanioDePagina);
	escribirBloqueMapeado(particion,pagina,dirFisNueva,tamanioDePagina);

}


///////FUNCIONES AUXILIARES/////

