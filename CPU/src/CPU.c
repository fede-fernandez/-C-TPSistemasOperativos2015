#include <stdio.h>
#include <stdlib.h>
#include "funcionesCPU.h"
#include <commonsDeAsedio/cliente-servidor.h>
#include <commonsDeAsedio/estructuras.h>

int main(void) {
	tipoConfigCPU* configuracion = cargarArchivoDeConfiguracionDeCPU("cfgCPU");//ruta de ejemplo

	bool cpuActiva = true;

	int socketParaPlanificador = crearSocket();

	int socketParaAdministrador = crearSocket();

	conectarAServidor(socketParaPlanificador,configuracion->ipPlanificador,configuracion->puertoPlanificador);

	conectarAServidor(socketParaAdministrador,configuracion->ipMemoria,configuracion->puertoMemoria);

	while(cpuActiva){//OJO aca meto un bool (linea 13) de negro caprichoso nomas, itera cmo  vos quieras el while

	tipoPCB pcbRecibido = recibirPCB(socketParaPlanificador);

////////////////ACA EMPIEZA FERNILANDIA/////////////////////////////////////////

	/******Comienzo de Actividad de CPU cuando Planificador envia mensaje*******/

		int instructionPointerActual = 0, instructionPointer = pcbRecibido.insPointer, instruccionBloqueante, pID = pcbRecibido.pid;

		char* instruccionActual, ruta = pcbRecibido.ruta;



		FILE* programa = fopen(ruta, "r");

		if(programa == NULL)
			printf("\nEl programa no existe o está vacío.\n");

		while(instructionPointerActual < instructionPointer) //IP me lo envía planificador por socket
		{
			fgets(instruccionActual, string_length(instruccionActual), programa);
			instructionPointerActual++;
		}

		while(fgets(instruccionActual, string_length(instruccionActual), programa) != NULL)
		{
			instruccionBloqueante = ejecutarInstruccion(instruccionActual, pID);
			//pID me lo envia el planificador por socket
			if(instruccionBloqueante == 1)
			{
				sleep(configuracion->retardo);
				break;
			}
			sleep(configuracion->retardo);
		}
	/******Fin de Actividad de CPU cuando Planificador envia mensaje*******/


/////////////////////////ACA TERMINA FERNILANDIA////////////////////////////////////

	tipoInstruccion instruccionAEnviar;// = algunaFuncion(); pd: no se cuando usas la estructura de instruccion

	void* bloqueConInstruccion = serializarInstruccion(instruccionAEnviar);

	enviarMensaje(socketParaAdministrador,bloqueConInstruccion,sizeof(bloqueConInstruccion));

	size_t tamanioRespuesta;

	void* bloqueDeRespuesta = recibirBloque(&tamanioRespuesta,socketParaAdministrador);

	tipoRespuesta respuestaDeAdministrador = deserializarRespuesta(tamanioRespuesta,bloqueDeRespuesta);

	//hacer algo con la respuesta

	}

	liberarSocket(socketParaPlanificador);
	liberarSocket(socketParaAdministrador);

	destruirConfigCPU(configuracion);
	return EXIT_SUCCESS;
}
