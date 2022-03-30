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

__msg_stage "Setup Environment"

__msg_stage "Update and Upgrade System"
sudo apt update && sudo apt -y upgrade

__msg_stage "Configure the Intel and Microsoft APT Repositories"
echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu bionic main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/18.04/prod bionic main" | sudo tee /etc/apt/sources.list.d/msprod.list
wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -

__msg_stage "Install the Intel and Open Enclave Host-Verify packages and dependencies"
sudo apt update
sudo apt -y install make cmake g++ libssl-dev libcurl4-openssl-dev libsgx-dcap-ql libsgx-dcap-ql-dev az-dcap-client open-enclave-hostverify

__msg_stage "Read and execute the content of openenclaverc"
echo "source /opt/openenclave/share/openenclave/openenclaverc" >> ~/.bashrc && source ~/.bashrc
export AZDCAP_DEBUG_LOG_LEVEL=None

__msg_stage "Build"

rm -rvf ./out
mkdir -pv out
export LOCAL_ROOT="$(dirname $(pwd))"
echo $LOCAL_ROOT
cd out/
cmake ../
make

./jwt-verifier

