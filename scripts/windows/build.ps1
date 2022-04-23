param (
     [string]$output_dir = "windows",       		# where to output the shadow build
	 [string]$project_name = "rex",	                # which project to build

     [switch]$clear = $false        				# Should we clear instead of building the soluction
)

# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

$cwd = Get-Location
$build_dir = "$cwd\build"

Write-Host "Executing windows/build.ps1" -ForegroundColor Green
Write-Host "Current working directory: $cwd"

Write-Host "Running script for Windows desktop"

Import-Module "$cwd\scripts\windows\msvc\clear_msvc.ps1"
Import-Module "$cwd\scripts\windows\msvc\build_msvc.ps1"

if (!(Test-Path "$build_dir"))
{	
    Invoke-Expression -Command "$cwd\scripts\windows\generate.ps1 -o=$output_dir"
}

if($clear)
{
    Write-Host "Start cleaning solution ($project_name) ..."
    clear_msvc $output_dir $project_name
}
else
{
    Write-Host "Start building solution ($project_name) ..."
    build_msvc $output_dir $project_name
}

Write-Host "Building script completed" -ForegroundColor green