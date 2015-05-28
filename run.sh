#!/bin/bash

SIZES[0]=1
SIZES[1]=16
SIZES[2]=128
SIZES[3]=512
SIZES[4]=1024
SIZES[5]=2048
SIZES[6]=4096

SIZES[7]=bool
SIZES[8]=int
SIZES[9]=long_long

container_size[0]=1000
container_size[1]=2000
container_size[2]=5000
container_size[3]=10000
container_size[4]=20000
container_size[5]=50000
container_size[6]=100000
container_size[7]=200000
container_size[8]=500000
container_size[9]=1000000

for CONTAINER in list vector map 
do
	echo $CONTAINER

	for ((CONTAINER_SIZE = 0; CONTAINER_SIZE < 10; CONTAINER_SIZE++ )); do
			
		echo container size: ${container_size[$CONTAINER_SIZE]}

		echo -n size 
		for ((SIZE = 0; SIZE < 10; ++SIZE))
		do
			echo -e -n "\t" ${SIZES[$SIZE]}
		done 
		echo

		for MEMORY in std fixed dynamic
		do
			echo -n $MEMORY

			for ((SIZE = 0; SIZE < 10; ++SIZE))
			do

				sum=0;

				for ((i = 0; i < $1; ++i))
				do
					var=$(./alf_test $CONTAINER $SIZE $MEMORY $CONTAINER_SIZE)
					let "sum+= $var"
				done

				# make it median

				let "sum /= $1"
				echo -e -n "\t" $sum
			done
			echo
		done
		echo
	done
	echo
done
