#!/bin/bash

rm -rvf ./out
mkdir out
export LOCAL_ROOT="$(dirname $(pwd))"
echo $LOCAL_ROOT
cd out/
cmake ../
# cmake  -DCMAKE_BUILD_TYPE=Debug ../
make

./jwt-verifier

