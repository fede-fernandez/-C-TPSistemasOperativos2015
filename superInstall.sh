#!/bin/bash

cd commonsDeAsedio/Debug

make clean

make

cd ..

sudo make install 

cd ..

cd Memoria/Debug

make clean

make

cd ..

cd ..

cd SWAP/Debug

make clean

make

cd ..

cd ..

cd CPU/Debug

make clean

make

cd ..

cd ..

cd Planificador/Debug

make clean

make

exit
