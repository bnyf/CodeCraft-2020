#! /bin/bash

workdir=`cd $(dirname $0); pwd`

cd $workdir
echo "start compile"
g++ -O3 main.cpp -o test -lpthread
echo "start test"
n=$1
if [ -d data/$n ]; then
    echo "-----$n-----"
    output=`./test $n`
    if [ -e data/$n/result.txt ]; then
        diff_output=`diff data/$n/result.txt data/$n/my_result.txt`
        if [ x"$diff_output" = x ]; then
            echo "Right Answer"
            echo "dfs,total time: $output"
        else
            echo "Wrong Answer"
        fi
    else
        echo "Unknown Answer"
        echo "dfs, total time: $output"
    fi
fi
