## References 

The code in this directory uses logic and, in some cases, copied/pasted from:
* Greg Kostal's https://github.com/gkostal/attestation/tree/master/sgx.attest.sample/validatequotes.net
* Larry Osterman's https://github.com/LarryOsterman/RetrieveAndVerifyMetadataCerts/tree/master/VerifyMetadataCertificates
* Martin Vorbrodt's blog on Base64 Encoding: https://vorbrodt.blog/2019/03/23/base64-encoding/
* Curl building using vcpkg: https://github.com/curl/curl/blob/master/docs/INSTALL.md#building-using-vcpkg
* LibCurl's example: https://curl.se/libcurl/c/getinmemory.html
* https://gist.github.com/cseelye/adcd900768ff61f697e603fd41c67625 <- TODO: add what this ref is

Thanks for the samples!

## Windows OS
#### Prerequisites
- [ ] Install cmake https://cmake.org/install/
- [ ] Install `msbuild`, see https://docs.microsoft.com/en-us/visualstudio/msbuild/msbuild?view=vs-2019 or Visual Studio
- [ ] Get the code: 
```
git clone --recursive git@github.com:olkroshk/attestation-sandbox.git
```
- [ ] Set up the development environment and install the dependencies:
```
cd attestation-sandbox\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
.\vcpkg.exe install curl[openssl] zlib openssl --triplet x64-windows
```

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

## Cheatsheet
Pull all submodules:
```
git submodule update --init --recursive
```
Update submodules:
```
git submodule update --recursive
```
