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
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..

for file in $args
do
    fileName=${file:0:-3}
    file="../$file"
    for alg in 1 2 3 4
    do
        algName=${algorithms[$alg]}
        for threads in 8 4 2 1
        do

            cd Chapel
            ./clear.sh
            export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
            echo $fileName $algName $threads
            ./chpl.out --file=$file --alg=$alg
            cd ..

        done
    done
done
