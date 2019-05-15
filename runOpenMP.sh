#!/usr/bin/env bash

args="*.dnn"



cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..


for file in $args
do
    dnn=`cat $file`
    file="../$file"
    for batch in 200 100 50
    do
        for threads in 8 4 2 1
        do

            cd OpenMP
            ./clear.sh
            echo "$dnn $batch ($threads)"

            ./openmp.out $file $batch $threads
            cd ..

        done
    done
done
