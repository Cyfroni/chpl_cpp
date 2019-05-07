#!/usr/bin/env bash



cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..


for file in *.dnn
do
    dnn=`cat $file`
    for batch in 100 25 5 1
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
