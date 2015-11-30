#!/bin/bash

cd Memoria/src

gcc algoritmos.c funcionesMemoria.c impresionesEnPantalla.c funcionesSeniales.c validaciones.c configuracion.c Memoria.c -o Memoria -lcommons -lcommonsDeAsedio -lpthread

cd ..

cd ..

cd SWAP/src

gcc funcionesSWAP.c huecosSWAP.c instruccionesSWAP.c particionSWAP.c SWAP.c -o SWAP -lcommons -lcommonsDeAsedio 

cd ..

cd ..

cd CPU/src

gcc funcionesCPU.c CPU.c -o CPU -lpthread -lcommons -lcommonsDeAsedio

cd ..

cd ..

cd Planificador/src

gcc funcionesPlanificador.c Planificador.c -o Planificador -lpthread -lcommons -lcommonsDeAsedio

exit
