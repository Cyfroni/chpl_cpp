#!/usr/bin/env bash

cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..

for n in 1024 2048
do
    for p in 32 16 8 4 2
    do
        for threads in 8 4 2 1
        do
            if [ "$threads" -ge "$p" ]; then
              cd OpenMP
              echo "$n $p ($threads)"
              ./openmp.out $n $p $threads
              cd ..
            fi
        done
    done
    cd OpenMP
    echo "$n 1 (1)"
    ./openmp.out $n 1 1
    cd ..
done
