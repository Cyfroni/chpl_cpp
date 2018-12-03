#!/usr/bin/env bash

cd OpenMP
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..

cd Chapel
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..

for n in 100
do
    for threads in 4
    do
#        cd OpenMP
#        ./openmp.out $n $threads
#        cd ..
#
       cd Chapel
       export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
       ./chpl.out
       cd ..

    done
done
