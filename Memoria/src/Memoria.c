#include <stdio.h>
#include <stdlib.h>
#include "funcionesMemoria.h"
#include <commonsDeAsedio/cliente-servidor.h>
#include <commons/collections/list.h>
#include <commonsDeAsedio/select.h>

#define maxConexionesEntrantes 10
#define puertoSwap 7204

int main(void) {

//////////////////////////INICIALIZACION DE VARIABLES////////////////////////////////

	tipoConfigMemoria* configuracion = cargarArchivoDeConfiguracionDeMemoria("/home/utnso/Escritorio/cfgMemoria");

	int socketParaCpus = crearSocket();

	int socketParaSwap = crearSocket();

	int socketCpuEntrante;

	bool memoriaActiva = true,hayCpuParaConexion = false;

	t_list* listaPrincipal = list_create();
	t_list* listaFiltrada = list_create();

	t_list* listaRAM;
	t_list* listaTLB;


/////////////////////////////////////////////////////////////////////////////////////

	asociarAPuerto(socketParaCpus,configuracion->puertoDeEscucha);

	asociarAPuerto(socketParaSwap,puertoSwap);//para mi en el archivo de configuracion deberia de
											 //estar el puerto de memoria para cnectarse a swap


	while(memoriaActiva){

		hayCpuParaConexion = filtrarListas(listaPrincipal,listaFiltrada);

		if(hayCpuParaConexion){
			socketCpuEntrante = crearSocketParaAceptarSolicitudes(socketParaCpus);
			cargarEnLista(listaPrincipal,socketCpuEntrante);
		}

		if(!list_is_empty(listaFiltrada))
		tratarPeticiones(socketParaCpus,socketParaSwap,listaFiltrada, listaTLB, listaRAM, configuracion);

	}

	destruirConfigMemoria(configuracion);

	return EXIT_SUCCESS;
}
