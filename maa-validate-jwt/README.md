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
*  .\vcpkg.exe install curl[core,openssl]:x64-windows  --recurse

```
.\vcpkg.exe install curl:x64-windows
```

## Windows OS
#### Prerequisites
- [ ] Install cmake https://cmake.org/install/
- [ ] VS tools
- [ ] Get the repo `git clone --recursive git@github.com:olkroshk/attestation-sandbox.git`
- [ ] Set up environment and install the dependencies:
	- [ ] Change into the `./vcpkg` submodule directory and build `vcpkg` packager: `.\bootstrap-vcpkg.bat`
	- [ ] Integrate `vcpkg` into the build environment: `.\vcpkg.exe integrate install`
	- [ ] Install the project's dependencies: `.\vcpkg.exe install curl[openssl] zlib openssl --triplet x64-windows`

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
