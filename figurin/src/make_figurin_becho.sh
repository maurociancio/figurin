#!/bin/bash

g++ -I ../../../cvblob/cvBlob/ main.cpp ../../../fann-2.1.0/src/.libs/libfann.a ../../../cvblob/lib/libcvblob.a  -lcv -lcvaux -lhighgui -o figurin
