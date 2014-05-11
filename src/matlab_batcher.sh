#!/bin/bash

matlab_exec=matlab
X="${1}(${2})"
tmpfile=`mktemp`
echo ${X} > $tmpfile
cat $tmpfile
${matlab_exec} -nojvm -nodisplay -nosplash < $tmpfile
rm $tmpfile
