#!/bin/bash
echo "*Compilando entrenador"
g++ training.cpp -I./libfann/fannsrc/include ./libfann/libfann.a -o trainer
echo "*Hecho"
echo "*Compilando Figurin"
 g++ main.cpp -I./libcvblob/ -I ./libfann/fannsrc/include/  -lcv  -lhighgui  ./libcvblob/libcvblob.a ./libfann/libfann.a -o figurin
echo "*Hecho"
