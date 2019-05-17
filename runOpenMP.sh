#!/usr/bin/env bash

cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh

for n in 512 1024
do
    for p in 32 16 8 4 2 1
    do
        for threads in 8 4 2 1
        do
            if [ "$p" -ge "$threads" ]; then

              echo "$n $p ($threads)"
              ./openmp.out $n $p $threads

            fi
        done
    done
done

cd ..
