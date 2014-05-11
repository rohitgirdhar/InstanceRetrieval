#!/bin/bash

# train
cd ../src/
NLeaves=10000
bash train.sh ../test/Images_bob/ ../test/bobIR/ 10 ${NLeaves}
cd ../test/

# Now use the ImageSearch program for search
./bobIR/ImageSearch -d ./bobIR/ -q Images_bob/00000000.jpg -l ${NLeaves}
# use --resize for faster search
./bobIR/ImageSearch -d ./bobIR/ -q Images_bob/00000000.jpg -l ${NLeaves} --resize
