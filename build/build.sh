#!/bin/bash
g++ -O3 -w ./generator.cpp -o generator.x 

cpu=`less /proc/cpuinfo | grep "Raspberry" | wc -l`

if [! -d "../src"]; then
    mkdir ../src
fi
if [! -f "../include"]; then
    mkdir ../include
fi

if [ $cpu -ge "1" ]; then
    echo "libjeff robotics branch."
    ./generator.x libjeff.index cpu+robotics
else
    echo "libjeff scientific branch."
    #    no_nvcc=`which nvcc 2>&1 | grep "no nvcc in" | wc -l`
    nvcc_out=`nvcc --version`
    if [ `echo ${nvcc_out} | grep "NVIDIA (R) Cuda compiler driver" | wc -l` -eq "1" ];then
	no_nvcc=0
    else
	no_nvcc=1
    fi
    
    if [ $no_nvcc -eq "1" ]; then
	echo $'No nvcc found in the system path.\nWill compile CPU code only.'
	./generator.x libjeff.index cpu+scientific
    else
	echo $'Using nvcc in '`which nvcc`$'\nBoth CPU and GPU code will be compiled.'
	./generator.x libjeff.index cpu+gpu+scientific+xgboost
    fi
fi

cd ../src
make -j16
