#!/bin/bash

if [ $# -lt 4 ]
then
    echo 'usage: ./train.sh [images_dir] [output_dir]'\
         '[branch_factor (for hikmeans)] [total_clusters (for hikmeans)]'
    exit -1
fi

IMAGES_DIR=${1}/
OUTPUT_DIR=${2}/
BRANCH_FACTOR=${3}
NUM_CLUSTERS=${4}

make
./getDescriptors -i ${IMAGES_DIR} -o ${OUTPUT_DIR} --resize

bash matlab_batcher.sh hikmeans \'${OUTPUT_DIR}\',${BRANCH_FACTOR},${NUM_CLUSTERS}
bash matlab_batcher.sh parseTree \'${OUTPUT_DIR}\',${NUM_CLUSTERS}
bash matlab_batcher.sh quantize \'${OUTPUT_DIR}\',${NUM_CLUSTERS}
bash matlab_batcher.sh wordsInDir \'${OUTPUT_DIR}\',${NUM_CLUSTERS}
bash combineWordsKeys.sh ${OUTPUT_DIR}
./buildSearchIndex -o ${OUTPUT_DIR}
mv ./ImageSearch $OUTPUT_DIR

rm -r ${OUTPUT_DIR}/MatFiles ${OUTPUT_DIR}/Descriptors.txt ${OUTPUT_DIR}/descriptors

