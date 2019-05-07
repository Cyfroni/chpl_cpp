#!/usr/bin/env bash

args="*.in"

algorithms=(
    null
    BF
    Gen
    SLF
    LLL
)

cd Chapel
chmod 755 compile.sh
./compile.sh
chmod 755 clear.sh
./clear.sh
cd ..

for file in $args
do
    file="../$file"
    for alg in 1 2 3 4
    do
        for threads in 4 2 1
        do

            cd OpenMP
            ./clear.sh
            echo $file $alg $threads
            ./openmp.out $file $alg $threads
            cd ..

        done
    done
done
