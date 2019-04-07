#!/usr/bin/env bash

args="*.data"
cnn="*.cnn"

cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..

cd Tools
python splitData.py
cd ..

for file in 1 #$args
do
    for CNN in 1 #$cnn
    do
        for threads in 1
        do

            cd OpenMP
            ./openmp.out $threads #$file $CNN $threads
            cd ..

        done
    done
done
