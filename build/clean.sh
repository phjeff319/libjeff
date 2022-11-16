#!/bin/bash
rm -f *.x
rm -f *~
for i in include src
do
    cd ../$i
    rm -f *.x
    rm -f *~
    rm -f *.cpp
    rm -f *.c
    rm -f *.cu
    rm -f *.h
    rm -f *.o
    rm -f *.a
    rm -f Makefile
done
