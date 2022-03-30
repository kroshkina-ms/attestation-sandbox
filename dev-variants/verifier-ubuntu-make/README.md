## Read Me First

- This sample variant uses libraries installed onto the system using `apt` package tool.

## Prerequisites
- Dev System with Ubuntu_18.04
- MAA JWT sample token as an input for the verification

## Install Dependencies

#### Update and Upgrade System
```
sudo apt update
```

```
sudo apt -y upgrade
```

#### Configure the Intel and Microsoft APT Repositories
> This step and the next one below are based on the Open Enclave's documentation with a few adjustments, see [install_host_verify_Ubuntu_18.04](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/install_host_verify_Ubuntu_18.04.md).

```bash
echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu bionic main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/18.04/prod bionic main" | sudo tee /etc/apt/sources.list.d/msprod.list
wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -

```

#### Install the Intel and Open Enclave Host-Verify packages and dependencies
```
sudo apt update
```

```
sudo apt -y install make g++ libssl-dev libcurl4-openssl-dev libsgx-dcap-ql libsgx-dcap-ql-dev az-dcap-client open-enclave-hostverify
```

> This step also installs the [az-dcap-client](https://github.com/microsoft/azure-dcap-client)
> package which is necessary for performing remote attestation in Azure. A general
> implementation for using Intel DCAP outside the Azure environment is coming soon.

#### Read and execute the content of `openenclaverc`
> This step is needed for ` pkg-config oehostverify-$(CXX) ...` command to function properly.

```
echo "source /opt/openenclave/share/openenclave/openenclaverc" >> ~/.bashrc && source ~/.bashrc
```

#### [*Optionally*]: Set Azure DCAP Debug Variable 
> This step is needed for supressing the warning message: Azure Quote Provider: libdcap_quoteprov.so [ERROR]: Could not retrieve environment variable for 'AZDCAP_DEBUG_LOG_LEVEL'

```
export AZDCAP_DEBUG_LOG_LEVEL=None
```

## Get Code
```
git clone --recursive git@github.com:kroshkina-msft/attestation-sandbox.git
```

```
cd attestation-sandbox
```

[*Optionally*]: When working with a branch:
```
git checkout <your_branch_name> 
git submodule update --init

```

## Build and Run
```
cd dev-variants/verifier-ubuntu-make
```

```
make clean && make
```

Check the tool's usage syntax:
```
./jwt-verifier
```

Verify quote in JWT:
```
./jwt-verifier [options] <jwt-filename>
```

For instance:
```
./jwt-verifier -v ~/samples/jwt.txt
```
