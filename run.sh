#!/usr/bin/env bash

#source chapel-1.18.0/util/setchplenv.bash

#cd Tools
#python prepare.py
#cd ..

args="*.data"
cnn="*.cnn"

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

#rm -rf results/
#mkdir results

for file in 1 #$args
do
    for CNN in 1 #$cnn
    do
        for threads in 4
        do

            cd OpenMP
            ./openmp.out #$file $CNN $threads
            cd ..

#            cd Chapel
#            export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
#            ./chpl.out #--file=$file
#            cd ..

        done
    done
done
