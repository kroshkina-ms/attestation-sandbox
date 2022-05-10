# Define Local Directories
#
$cur_dir=Get-Location
# TODO fix the local dir
$Env:LOCAL_ROOT="$cur_dir\..\.."
$tmp_dir = "$cur_dir\tmp"
If((Test-Path $tmp_dir))
{
    Remove-Item $tmp_dir -Force -Recurse
}
New-Item -ItemType Directory -Force -Path $tmp_dir

# Download Nuget Tool
#
$nuget_file_name = "nuget.exe"
$nuget_source = "https://dist.nuget.org/win-x86-commandline/latest/$nuget_file_name"
$nuget_destination = "$tmp_dir\$nuget_file_name"
Invoke-WebRequest -Uri $nuget_source -OutFile $nuget_destination
dir $tmp_dir
$nuget_exe = "$nuget_destination"

# Download and Install OE Nuget Packages
#
$oe_version = "0.17.7"
$oe_name = "open-enclave.OEHOSTVERIFY"
$oe_nupkg_name = "$oe_name.$oe_version.nupkg"
$oe_source = "https://github.com/openenclave/openenclave/releases/download/v$oe_version/$oe_nupkg_name"
echo "OE Source = $oe_source"
$oe_destination = "$tmp_dir\$oe_nupkg_name"
Invoke-WebRequest -Uri $oe_source -OutFile $oe_destination
dir $tmp_dir

$oe_output_directory = "$tmp_dir\oe_installed_nupkg"
$oe_nuget_args = @('install', $oe_name, '-Source', $tmp_dir, '-OutputDirectory', $oe_output_directory, '-ExcludeVersion')
$oe_path = "C:\$oe_name"
$oe_xcopy_args = @('/D', '/Y', '/E', "$oe_output_directory\$oe_name\openenclave", $oe_path)
& $nuget_exe $oe_nuget_args
xcopy $oe_xcopy_args
dir $oe_path

# Download and Install MS Azure DCAP Nuget Packages
#
$msdcap_version = "1.10.0"
$msdcap_name = "Microsoft.Azure.DCAP"
$msdcap_nupkg_name = "$msdcap_name.$msdcap_version.nupkg"
$msdcap_source = "https://www.nuget.org/api/v2/package/Microsoft.Azure.DCAP/$msdcap_version"
$msdcap_destination = "$tmp_dir\$msdcap_nupkg_name"
Invoke-WebRequest -Uri $msdcap_source -OutFile $msdcap_destination
dir $tmp_dir

$msdcap_output_directory = "$tmp_dir\msdcap_installed_nupkg"
$msdcap_nuget_args = @('install', $msdcap_name, '-Source', $tmp_dir, '-OutputDirectory', $msdcap_output_directory, '-ExcludeVersion')
$msdcap_path = 'C:\azure_dcap'
$msdcap_nuget_path = "$msdcap_path\$msdcap_name"
& $nuget_exe $msdcap_nuget_args
$msdcap_xcopy_args = @('/d', '/Y', '/E', $msdcap_output_directory, $msdcap_path)
xcopy $msdcap_xcopy_args
# Install DCAP nuget
cd "$msdcap_nuget_path\tools"
& ".\InstallAzureDCAP.ps1" "$msdcap_nuget_path\build\native\Library"
dir $msdcap_path
dir "$msdcap_nuget_path\build\native\Library"

# Build and Install vcpkg Dependencies
#
#cd $cur_dir\vcpkg
$vcpkg_dir="$cur_dir\..\..\vcpkg"

cd $vcpkg_dir
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
.\vcpkg.exe install curl[openssl] openssl --triplet x64-windows

$project_dir="$cur_dir"

cd $project_dir
$project_out = "out"
If((Test-Path $project_out))
{
    Remove-Item $project_out -Force -Recurse
}
New-Item -ItemType Directory -Force -Path $project_out

cd $project_out 

cmake -DCMAKE_PREFIX_PATH="$oe_path\lib\openenclave\cmake" -DCMAKE_TOOLCHAIN_FILE="$vcpkg_dir/scripts/buildsystems/vcpkg.cmake" -DNUGET_PACKAGE_PATH="$msdcap_nuget_path" $project_dir

$msbuild_path=$Args[0]
$msbuild_exe=""
If($msbuild_path -eq $null) {
    $msbuild_file_name = (Get-ChildItem -Recurse -Path "C:\Program Files (x86)\Microsoft Visual Studio\" -Include "msbuild.exe").fullname | Select -First 1
    $msbuild_path = Split-Path -Path "$msbuild_file_name"
    $msbuild_exe = (Get-ChildItem -Recurse -Path "C:\Program Files (x86)\Microsoft Visual Studio\" -Include "msbuild.exe").fullname | Select -First 1
}

#TODO fixme
#echo ">>> Temporary adding '$msbuild_path' to the PATH"
#Set-Item -Path Env:Path -Value ($Env:Path + $msbuild_path)
& $msbuild_exe jwt-verifier.sln

cd .\Debug
.\jwt-verifier.exe
cd $cur_dir
