/*
 * funcionesSeñales.c
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */
#include "funcionesSeñales.h"

void funcionPrueba(tipoEstructuraMemoria* datosMemoria){

	/*int boton = 0;

	while(true){

		scanf("%d",&boton);

		switch (boton) {
			case 1:*/
				mostrarTablasDePaginas(datosMemoria);
				printf("\n\n");
				mostrarTLB(datosMemoria);
				printf("\n\n");
				/*break;
		}

	}*/

}

void mostarEstado(tipoEstructuraMemoria* datosMemoria){

	mostrarTablasDePaginas(datosMemoria);
	printf("\n\n");
	mostrarTLB(datosMemoria);
	printf("\n\n");
}


void mostrarTablaDePaginas(int posicionDeTabla,tipoEstructuraMemoria* datosMemoria){

	tipoTablaPaginas* tablaPaginas = list_get(datosMemoria->listaTablaPaginas,posicionDeTabla);

	printf(" _________________________ \n");
	printf("|PID|PAGPED|PAGASIG|FRAMES|\n");
	printf("|___|______|_______|______|\n");
	printf("|%d |%d     |%d      |%d     |\n",tablaPaginas->pid,tablaPaginas->paginasPedidas,tablaPaginas->paginasAsignadas,list_size(tablaPaginas->frames));
	printf("|___|______|_______|______|\n");
}

void mostrarTablasDePaginas(tipoEstructuraMemoria* datosMemoria){

	printf("######################TABLA DE PAGINAS############################");


	int var;
	for (var = 0; var < list_size(datosMemoria->listaTablaPaginas); ++var) {

		mostrarTablaDePaginas(var,datosMemoria);

	}

	printf("#################################################################");

}

void mostrarTipoTLB(int posicionDeTabla,tipoEstructuraMemoria* datosMemoria){


	tipoTLB* tablaPaginas = list_get(datosMemoria->listaTLB,posicionDeTabla);

	printf(" _________________________ \n");
	printf("|PID|NROPAG|POSRAM|\n");
	printf("|___|______|_______|\n");
	printf("|%d |%d     |%d      |\n",tablaPaginas->pid,tablaPaginas->numeroDePagina,tablaPaginas->posicionEnRAM);
	printf("|___|______|_______|\n");
}

void mostrarTLB(tipoEstructuraMemoria* datosMemoria){

	printf("######################TLB#######################################");

	int var;
	for (var = 0; var < list_size(datosMemoria->listaTLB); ++var) {

		mostrarTipoTLB(var,datosMemoria);

	}

	printf("################################################################");
}

