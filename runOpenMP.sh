#!/usr/bin/env bash



cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..


for file in "*.dnn"
do
    for batch in 1 5 25 100
    do
        for threads in 8 4 2 1
        do

            cd OpenMP
            ./clear.sh
            echo "$file $batch ($threads)"

            ./openmp.out $file $alg $threads
            cd ..

        done
    done
done
