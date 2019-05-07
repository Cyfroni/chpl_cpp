#!/usr/bin/env bash

source chapel-1.18.0/util/setchplenv.bash

cd Chapel
chmod 755 compile.sh
./compile.sh || exit 1
chmod 755 clear.sh
./clear.sh
cd ..


for threads in 8 4 2 1
do
    cd Chapel
    export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
    ./chpl.out
    cd ..
done
