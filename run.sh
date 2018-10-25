#!/usr/bin/env bash

#source chapel-1.18.0/util/setchplenv.bash

cd Tools
python prepare.py
cd ..

args="*.data"
cnn="*.cnn"

cd OpenMP
chmod 755 compile.sh
./compile.sh
chmod 755 clear.sh
./clear.sh
cd ..

cd Chapel
chmod 755 compile.sh
./compile.sh
chmod 755 clear.sh
./clear.sh
cd ..

rm -rf results/
mkdir results

for file in $args
do
    for CNN in $cnn
    do
        for threads in 4 3 2 1
        do

            cd OpenMP
            ./clear.sh
            ./openmp.out $file $CNN $threads
    #        openmp=$(cat time.txt)
            cd ..

    #        cd Chapel
    #        ./clear.sh
    #        export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
    #        ./chpl.out --file=$file
    #        chapel=$(cat time.txt)
    #        cd ..

        done
    done
done