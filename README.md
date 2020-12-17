
## About

Sandbox!

## MAA JWT Validation Tool

Source code and readme for the tool are located in [./maa-validate-jwt](https://github.com/olkroshk/attestation-sandbox/tree/main/maa-validate-jwt)

### Build and Run - Ubuntu Linux

- [ ] Get code 
```
git clone --recursive https://github.com/olkroshk/attestation-sandbox.git
cd ./attestation-sandbox
```

- [ ] Set up environment and install dependencies
```
./ubuntu-bootstrap-mvj.sh
```

- [ ] Run `maa-validate-jwt`
```
cd ./maa-validate-jwt/out
./maa-validate-jwt ARGS 
```

### Build and Run - Windows

- [ ] Install cmake https://cmake.org/install/
- [ ] Install `msbuild`, see https://docs.microsoft.com/en-us/visualstudio/msbuild/msbuild?view=vs-2019 or Visual Studio 2019
- [ ] Get code 
```
git clone --recursive https://github.com/olkroshk/attestation-sandbox.git
cd .\attestation-sandbox
```

- [ ] Set up environment and install dependencies
```
.\windows-bootstrap-mvj.ps1
```

- [ ] Run `maa-validate-jwt`
```
cd .\maa-validate-jwt\out\Debug\
.\maa-validate-jwt.exe ARGS 
```

