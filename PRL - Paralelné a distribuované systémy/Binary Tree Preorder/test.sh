#!/bin/bash

tree=$1
cpuNum=$((${#tree}*2-2))

if [ $cpuNum = 0 ]; then
    cpuNum=1
fi

nodeNum=$((${#tree}-1))

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o pro pro.cpp

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $cpuNum pro $tree $nodeNum

#uklid
rm -f pro
