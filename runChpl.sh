#!/usr/bin/env bash

source chapel-1.18.0/util/setchplenv.bash

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

            cd Chapel
            ./clear.sh
            export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
            echo $file $alg $threads
            ./chpl.out --file=$file --alg=$alg
            cd ..

        done
    done
done
