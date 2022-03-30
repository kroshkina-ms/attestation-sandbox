#!/bin/bash

# abort on nonzero exitstatus
set -o errexit
# abort on unbound variable
set -o nounset   
# do not hide errors within pipes
set -o pipefail  

function __msg() {
    echo -e "    $*"
}

function __msg_stage() {
    echo -e "********************************************************************"
    echo -e "*   $*"
    echo -e "********************************************************************"
}

__msg_stage "Install tools"

sudo apt update && sudo apt -y upgrade
sudo apt install -y make cmake g++ zip

__msg_stage "Set up development environment and install the dependencies"

export LOCAL_ROOT=$(pwd)
__msg "LOCAL_ROOT is set to $LOCAL_ROOT"

export VCPKG_ROOT=$LOCAL_ROOT"/../../vcpkg"
__msg "VCPKG_ROOT is set to $VCPKG_ROOT"

git submodule update --init --recursive

cd $VCPKG_ROOT
__msg "Running ./bootstrap-vcpkg.sh"
./bootstrap-vcpkg.sh -disableMetrics

__msg "Running ./vcpkg integrate install"
mkdir -pv ~/.vcpkg
ls -la ~/.vcpkg
./vcpkg integrate install

__msg "Running ./vcpkg install curl[openssl] openssl --triplet x64-linux"
./vcpkg install curl[openssl] openssl --triplet x64-linux
cd $LOCAL_ROOT

__msg_stage "Clean and build MAA JWT validation tool"

JWT_VALIDATOR_SRC_DIR=$LOCAL_ROOT
JWT_VALIDATOR_OUT_DIR=$JWT_VALIDATOR_SRC_DIR"/out"

cd $JWT_VALIDATOR_SRC_DIR
rm --recursive --force --verbose $JWT_VALIDATOR_OUT_DIR
mkdir --verbose --parents $JWT_VALIDATOR_OUT_DIR
cd $JWT_VALIDATOR_OUT_DIR
cmake $JWT_VALIDATOR_SRC_DIR
make

__msg "MAA JWT validation tool was built. Executable is located in $JWT_VALIDATOR_OUT_DIR"
ls -l --all
./jwt-verifier

