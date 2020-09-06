#!/bin/bash

numCount=$1
cpuNum=$2

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o mss mss.cpp

#vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=$numCount of=numbers 2> /dev/null

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $cpuNum mss $numCount

#uklid
rm -f mss numbers
