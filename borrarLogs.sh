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

if [ -f "logActividadDeCPUs" ]; then rm "logActividadDeCPUs"
fi

if [ -f "logComienzoYFinDeMproc" ]; then rm "logComienzoYFinDeMproc"
fi

if [ -f "logColaDeReady"   ]; then rm "logColaDeReady"
fi

if [ -f "logMetricas" ]; then rm "logMetricas"
fi

if [ -f "logResultadosDeRafagas" ]; then rm "logResultadosDeRafagas"
fi

exit
