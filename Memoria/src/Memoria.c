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
	t_list* listaLectura = list_create();
	t_list* listaEscritura = list_create();
/////////////////////////////////////////////////////////////////////////////////////

	asociarAPuerto(socketParaCpus,configuracion->puertoDeEscucha);

	asociarAPuerto(socketParaSwap,puertoSwap);//para mi en el archivo de configuracion deberia de
											 //estar el puerto de memoria para cnectarse a swap


	while(memoriaActiva){

		hayCpuParaConexion = filtrarListas(listaPrincipal,listaLectura,listaEscritura);

		if(hayCpuParaConexion){
			socketCpuEntrante = crearSocketParaAceptarSolicitudes(socketParaCpus);
			cargarEnLista(listaPrincipal,socketCpuEntrante);
		}

		tratarLecturas(socketParaCpus,socketParaSwap,listaLectura);

		tratarEscrituras(socketParaCpus,socketParaSwap,listaEscritura);
	}

	destruirConfigMemoria(configuracion);

	return EXIT_SUCCESS;
}
