#!/usr/bin/env bash

cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..

for n in 512 1024
do
    for p in 32 16 8 4 2 1
    do
        for threads in 8 4 2 1
        do
            if [ "$p" -ge "$threads" ]; then
              cd OpenMP
              echo "$n $p ($threads)"
              ./openmp.out $n $p $threads
              cd ..
            fi
        done
    done
done
