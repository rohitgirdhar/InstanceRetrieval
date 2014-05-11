InstanceRetrieval
=================

Original Author : [Jay Panda](https://github.com/jaypanda "github")

Dependencies
------------
1. OpenCV
2. Boost C++ Libraries
3. VLFeat (included in this distribution as a submodule)

Usage
-----

```bash
$ git clone git@github.com:rohitgirdhar/InstanceRetrieval.git
$ cd InstanceRetrieval

## Obtain VLFeat
$ git submodule init
$ git submodule update
# compile VLFeat
$ cd src/vlfeat
$ make MEX=${MATLAB_ROOT}/bin/mex
$ cd ../..

## Train
$ cd src
$ bash Train.sh [..options..] # a sample in demo file
```
