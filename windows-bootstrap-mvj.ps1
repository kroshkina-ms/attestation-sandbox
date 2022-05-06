$curdir = Get-Location
$Env:LOCAL_ROOT=$curdir

Install-Module -Name Invoke-MsBuild

git submodule update --init --recursive

cd $curdir\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
.\vcpkg.exe install curl[openssl] zlib openssl --triplet x64-windows

$mvjdir="$curdir/maa-validate-jwt"

cd $mvjdir
$out = "out"
If((Test-Path $out))
{
    Remove-Item $out -Force -Recurse
}
New-Item -ItemType Directory -Force -Path $out

cd $out 

cmake $mvjdir

$msbuildpath=$Args[0]

If($msbuildpath -eq $null) {
    $msbuildpath = ";C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin"
}

echo "Temporary adding '$msbuildpath' to the PATH"

Set-Item -Path Env:Path -Value ($Env:Path + $msbuildpath)
msbuild maa-validate-jwt.sln

cd .\Debug
.\maa-validate-jwt.exe
cd $curdir
