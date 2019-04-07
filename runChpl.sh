#!/usr/bin/env bash

source chapel-1.18.0/util/setchplenv.bash

args="*.data"
cnn="*.cnn"

cd Chapel
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..

cd Tools
python splitData.py
cd ..

for file in 1 #$args
do
    for CNN in 1 #$cnn
    do
        for threads in 1
        do

            cd Chapel
            export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
            ./chpl.out #--file=$file
            cd ..

        done
    done
done
