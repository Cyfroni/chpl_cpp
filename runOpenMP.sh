#!/usr/bin/env bash

cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..


for threads in 8 4 2 1
do
    cd OpenMP
    ./openmp.out $threads
    cd ..
done
