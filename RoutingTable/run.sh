cd OpenMP
sh compile.sh
cd ..

cd Chapel
sh compile.sh
cd ..

num=1

for file in "$@"
do
    #mkdir -p "${file}_${num}"
    for alg in 1 2 3 4
    do

        for threads in 1 2 3 4 5 6 7 8
        do
            cd OpenMP
            sh clear.sh
            ./openmp.out $file $alg $threads
            #openmp = cat result.txt >> ../result.txt
            cd ..

            cd Chapel
            sh clear.sh
            export CHPL_RT_NUM_THREADS_PER_LOCALE=$threads
            ./chpl.out --file=$file --alg=$alg
            #cat result.txt >> ../result.txt
            cd ..
        done

    done
done