/*
 * funcionesSeñales.c
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */
#include "impresionesEnPantalla.h"

void funcionPrueba(tipoEstructuraMemoria* datosMemoria){

	int boton = 0;

	while(true){

		scanf("%d",&boton);

		switch (boton) {
			case 0:
				mostrarTablasDePaginas(datosMemoria);
				printf("\n\n");
				imprimirTablas(datosMemoria->listaTablaPaginas);
				printf("\n\n");
				//mostrarTLB(datosMemoria);
				mostrarRAM(datosMemoria->listaRAM);
				printf("\n\n");
				break;
/*
			case 2:
				limpiarRam();
				break;

			case 3:
				//scanf("%d",&boton);
				mostrarTasasPageFaults(datosMemoria);
				break;
			case 4:
				volcarRamALog();
				break;*/
		}

	}

}

void mostrarTasasPageFaults(tipoEstructuraMemoria* datosMemoria){

	int var;
	for (var = 0; var < list_size(datosMemoria->listaTablaPaginas); ++var) {

		mostrarTasaPageFaults(var);
	}
}

void mostrarTasaPageFaults(int posicion){

	tipoTablaPaginas* tabla = list_get(datosMemoria->listaTablaPaginas,posicion);

	printf("\nLa tasa de fallos de paginas del proceso %d es de %d / %d \n",tabla->pid,tabla->cantidadDePageFaults,tabla->cantidadDeAccesos);
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

void mostrarRAM(t_list* ram){

	printf("MOSTRANDO RAM:\n");

	int var;
	for (var = 0; var < list_size(ram); ++var) {

		printf("MARCO %d :\n",var);
		if(*((bool*)list_get(datosMemoria->listaHuecosRAM,var)))
			printf("ESTA MARCADO COMO HUECO\n");

		printf("%s\n",(char*)list_get(ram,var));

	}
}

void imprimirTablas(t_list* tablas){

	tipoTablaPaginas* tabla;

	int var;
	for (var = 0; var < list_size(tablas); ++var) {

		tabla = list_get(tablas,var);

		printf("TABLA DE PAGINA DE %d:\n",tabla->pid);

		printf("CON ACCESOS: ");
		int i;
		for (i = 0; i < list_size(tabla->listaParaAlgoritmo); ++i) {
			printf("%d | ",(int)(*(int*)list_get(tabla->listaParaAlgoritmo,i)));
		}

		printf("\n");

		printf("punteroDeAlgoritmo: %d\n\n",tabla->punteroParaAlgoritmo);


		imprimirPaginas(tabla->frames);
	}
}

void imprimirPaginas(t_list* frames){
	printf("PAGINAS:\n");

	int var;
	for (var = 0; var < list_size(frames); ++var) {

		tipoPagina* paginaAux = list_get(frames, var);

		printf("Nº: "); printf("%d		",var);
		printf("Posicion RAM: "); printf("%d		",paginaAux->posicionEnRAM);
		printf("Modificado: "); printf("%d		",paginaAux->modificado);
		printf("\n\n");

	}

}

void mostrarTasaTLBPeriodicamente(){

	while(true){

			sleep(60);//Espero 1 minuto

			log_trace(datosMemoria->logDeTLB,"LA TASA DE ACIERTOS DE LA TLB ES DEL %.1f%%",tasaAciertosTLB());

		}

}

