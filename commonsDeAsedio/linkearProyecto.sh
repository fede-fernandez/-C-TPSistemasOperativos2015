#!/bin/bash
# -*- ENCODING: UTF-8 -*-

cd

cd workspace/commonsDeAsedio/Debug

make clean

make

cd

cd workspace/$1/Debug/

make clean

make

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/commonsDeAsedio/Debug/

clear

./$1

exit
