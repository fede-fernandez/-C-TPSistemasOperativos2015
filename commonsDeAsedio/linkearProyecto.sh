#!/bin/bash
# -*- ENCODING: UTF-8 -*-

cd

cd  git/tp-2015-2c-los-javimancos/commonsDeAsedio/Debug

make clean

make

cd

cd tp-2015-2c-los-javimancos/$1/Debug/

make clean

make

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/tp-2015-2c-los-javimancos/commonsDeAsedio/Debug/

clear

./$1

exit
