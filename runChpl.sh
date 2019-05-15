#!/usr/bin/env bash

args="*.dnn"

source chapel-1.18.0/util/setchplenv.bash

cd Chapel
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..


for file in $args
do
    dnn=`cat $file`
    file="../$file"
    for batch in 200 100 50
    do
        for threads in 8 4 2 1
        do

            cd Chapel
            ./clear.sh
            echo "$dnn $batch ($threads)"
            export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
            ./chpl.out --_file=$file --batch=$batch
            cd ..

        done
    done
done
