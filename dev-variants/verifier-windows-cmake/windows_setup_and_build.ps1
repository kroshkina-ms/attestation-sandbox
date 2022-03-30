$curdir = Get-Location
$LOCAL_ROOT=$curdir

git submodule update --init --recursive

cd $LOCAL_ROOT
$out = "out"
If((Test-Path $out))
{
    Remove-Item $out -Force -Recurse
}
New-Item -ItemType Directory -Force -Path $out

cd $out 

cmake $LOCAL_ROOT

$msbuildpath=$Args[0]

If($msbuildpath -eq $null) {
    $msbuildpath = ";C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin"
}

echo "Temporary adding '$msbuildpath' to the PATH"

Set-Item -Path Env:Path -Value ($Env:Path + $msbuildpath)
msbuild jwt-verifier.sln

cd .\Debug
.\jwt-verifier.exe
cd $curdir
