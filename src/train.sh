#!/bin/bash

if [ $# -lt 4 ]
then
    echo 'usage: ./train.sh [images_dir] [output_dir]'\
         '[branch_factor (for hikmeans)] [total_clusters (for hikmeans)]'
    exit -1
fi

OUTPUT_DIR=${2}
BRANCH_FACTOR=${3}
NUM_CLUSTERS=${4}

make
#./getDescriptors -i $1 -o $2

# prefer run the following on atom (server)
bash matlab_batcher.sh hikmeans \'${OUTPUT_DIR}\',${BRANCH_FACTOR},${NUM_CLUSTERS}
bash matlab_batcher.sh parseTree \'${OUTPUT_DIR}\',${NUM_CLUSTERS}
