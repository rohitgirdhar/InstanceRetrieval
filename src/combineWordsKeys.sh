#!/bin/bash
dir=$1
list=`cat ${dir}/ImageNames.txt`
mkdir -p ${dir}/wordkeys
for i in $list
do
    paste ${dir}/words/w_${i}.txt ${dir}/keypoints/K_${i}.txt > ${dir}/wordkeys/wk_$i.txt
done
