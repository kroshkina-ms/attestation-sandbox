## References 

The code in this directory is copied/pasted and reimplemented from:
* Greg Kostal's https://github.com/gkostal/attestation/tree/master/sgx.attest.sample/validatequotes.net
* Larry Osterman's https://github.com/LarryOsterman/RetrieveAndVerifyMetadataCerts/tree/master/VerifyMetadataCertificates

Thanks to Larry and Greg for the samples!

External sources:
* Martin Vorbrodt's blog on Base64 Encoding: https://vorbrodt.blog/2019/03/23/base64-encoding/

cURL lib:
* https://github.com/curl/curl/blob/master/docs/INSTALL.md#building-using-vcpkg
* https://github.com/Microsoft/vcpkg/#examples
```
.\vcpkg.exe install curl:x64-windows
```

## Windows OS
#### Prerequisites
- [ ] Install cmake https://cmake.org/install/
- [ ] VS tools
- [ ] Install `libcurl` https://github.com/curl/curl/blob/master/docs/INSTALL.md#building-using-vcpkg


#### Build and Run
```
.\windows_build_and_run.ps1
```
or
```
.\windows_build_and_run.ps1 PATH_TO_MSBUILD.EXE
```

## Linux
#### Prerequisites
ubuntu linux
```
sudo apt install -y cmake make g++
```
TODO: add command for installation of libcurl
#### Build and Run
```
sudo chmod +x linux_build_and_run.sh
./linux_build_and_run.sh
```
