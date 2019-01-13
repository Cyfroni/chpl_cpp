#!/usr/bin/env bash

cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..

for n in 200 400
do
    for p in 20 10 5 2 1
    do
        for threads in 2 1
	      do
        	 cd OpenMP
        	 ./openmp.out $n $p $threads
        	 cd ..
	      done
    done
done
