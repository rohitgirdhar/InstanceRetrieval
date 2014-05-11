InstanceRetrieval
=================

Original Author : [Jay Panda](https://github.com/jaypanda "github")

Usage
-----

```bash
$ git clone git@github.com:rohitgirdhar/InstanceRetrieval.git
$ cd InstanceRetrieval/src

## Obtain VLFeat
$ git submodule
# compile VLFeat
$ cd vlfeat
$ make MEX=${MATLAB_ROOT}/bin/mex
$ cd ..

## Train
$ bash Train.sh [..options..] # a sample in demo file
```
