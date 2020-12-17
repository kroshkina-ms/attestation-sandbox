#!/bin/bash

# TODO: 
# 1. set variable output directory
# 2. Check if exists
# 3
rm -rvf ./out
# 4
mkdir out
# 5. check if was created
export LOCAL_ROOT="$(dirname $(pwd))"
echo $LOCAL_ROOT
cd out/
cmake ../
make

./maa-validate-jwt
