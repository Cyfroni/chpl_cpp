#!/usr/bin/env bash

#source chapel-1.18.0/util/setchplenv.bash
args="*.in"

algorithms=(
    null
    BF
    Gen
    SLF
    LLL
)

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
    fileName=${file:0:-3}
    timeFile="results/$fileName"_time".txt"
    printf "\n\t\t\t$fileName\n"
    printf "\t OpenMP\t\t\tChapel\n"
    printf "\t OpenMP\t\t\tChapel\n" >> $timeFile
    file="../$file"

    for alg in 1 2 3 4
    do
        algorithmName=${algorithms[$alg]}
        pathForNodes="results/${fileName}_${algorithmName}"

        printf $algorithmName':\n'
        printf $algorithmName':\n' >> $timeFile

        mkdir -p ${pathForNodes}

        for threads in 4 2 1
        do
            errorMessage=''

            cd OpenMP
            ./clear.sh
            ./openmp.out $file $alg $threads
            openmp=$(cat time.txt)
            cd ..
#            echo $openmp

            cd Chapel
            ./clear.sh
            export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
            ./chpl.out --file=$file --alg=$alg
            chapel=$(cat time.txt)
            cd ..

            cd OpenMP
            for entry in *.node
            do
              openMPFile=$entry
              chapelFile=../Chapel/"$entry"
              difference=$(diff "$openMPFile" "$chapelFile")

              if [ -n "$difference" ]; then
                errorMessage="\t<-- mismatch"
                printf "$difference" > ../$pathForNodes/$entry"_diff"

                mp=$(sed "s/:[[:digit:]]*//" "$openMPFile")
                chpl=$(sed "s/:[[:digit:]]*//" "$chapelFile")
                errorDifference=$(diff <(printf "$mp") <(printf "$chpl"))

                if [ -n "$errorDifference" ]; then
                    errorMessage="\t<-- error"
                    printf "$errorDifference" > ../$pathForNodes/$entry"_error"
                fi

                mv $openMPFile ../$pathForNodes/$entry"_OpenMP"
                mv $chapelFile ../$pathForNodes/$entry"_Chapel"

              else
                mv $openMPFile ../$pathForNodes
                rm $chapelFile
              fi
            done
            cd ..

#            result="\t$threads: $openmp\t;\t$chapel $errorMessage\n"
            printf "\t$threads: $openmp\t;\t$chapel $errorMessage\n"
            printf "\t$threads: $openmp\t;\t$chapel $errorMessage\n" >> $timeFile
        done
    done
done