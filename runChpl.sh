#!/usr/bin/env bash

source chapel-1.18.0/util/setchplenv.bash

for n in 1024 2048
do
    for p in 32 16 8 4 2 1
    do
        for threads in 8 4 2 1
        do
            if [ "$p" -ge "$threads" ]; then

              cd Chapel
              chmod 755 compile.sh
              ./compile.sh -s n=$n p=$p || exit 1
              chmod 755 clear.sh
              ./clear.sh
              
              export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
              echo "$n $p ($threads)"
              ./chpl.out
              cd ..

            fi
        done
    done
done
