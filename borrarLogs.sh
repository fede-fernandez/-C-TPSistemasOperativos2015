#!/bin/bash

cd Memoria/Debug

if [ -f logDeTLB ]; then rm logDeTLB 
fi

if [ -f logDeSwapeo ]; then rm logDeSwapeo
 fi

if [ -f logDeSeniales ]; then rm logDeSeniales
 fi

if [ -f logDeAlgoritmos ]; then rm logDeAlgoritmos 
fi

if [ -f logDeMemoria ]; then rm logDeMemoria 
fi

cd ..

cd ..

cd SWAP/Debug

if [ -f logSWAP ]; then rm logSWAP
fi

cd ..

cd ..

cd CPU/Debug

if [ -f logsCPU ]; then rm logsCPU
fi

cd ..

cd ..

cd Planificador/Debug

if [ -f "actividad CPUs" ]; then rm "actividad CPUs"
fi

if [ -f "comienzo y fin de mproc" ]; then rm "comienzo y fin de mproc"
fi

if [ -f "cola de ready"   ]; then rm "cola de ready"
fi

if [ -f metricas ]; then rm metricas          
fi

if [ -f "resultado de Ráfagas" ]; then rm "resultado de Ráfagas"
fi

exit
