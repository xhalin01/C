#!/bin/bash

numCount=$1

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o bks bks.cpp

#vyrobeni souboru s random cisly
dd if=/dev/urandom bs=1 count=$numCount of=numbers 2> /dev/null



#vypocet potrebnenho poctu cpu
if [ "$numCount" -eq 1 ]; then
	cpuNum=1
	treeCapacity=2
elif [ "$numCount" -eq 2 ]; then
	cpuNum=1
	treeCapacity=2
else 
	#vypocet najblizsej mocniny 2 k danemu cislu numCount
	cpuNum=1
	while [ $cpuNum -lt $numCount ]; do
		cpuNum=$(($cpuNum<<1))
		treeCapacity=$cpuNum
	done

	#vypocet logaritmu
	temp=$(python3 -c "import math; print(math.ceil(math.log($cpuNum)))")


	#vypocet najblizsej mocniny 2
	cpuNum=1
	while [ $cpuNum -lt $temp ]; do
		cpuNum=$(($cpuNum<<1))
	done

	#vypocet potrebnych uzlov stromu
	cpuNum=$(python3 -c "import math; print($cpuNum*2-1)")

fi

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $cpuNum bks $numCount $treeCapacity

#uklid
rm -f bks numbers
