#!/bin/bash

g++ main.cpp -I ../../../fann-2.1.0/src/include/ -I ../../../cvblob/cvBlob/ -lcv -lcvaux -lhighgui ../../../cvblob/lib/libcvblob.a ../../../fann-2.1.0/src/.libs/libfann.a -o figurin
