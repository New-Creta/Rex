param (
     [string]$output_dir = "windows",       # where to output the shadow build
     [string]$app_name,                     # which app to run

     [switch]$help = $false                 # help message
)

if($help)
{
    Write-Host "Arguments start with a hyphen ' - ' and consist out of the following options:"
    Write-Host ""
    Write-Host "output_dir------------default value: windows     # where to output the shadow build"
    Write-Host "app_name (mandatory)--default value: none        # which app to run"

    Write-Host "help------------------default value: false       # print this message"
    exit
}

# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

$cwd = Get-Location
$build_dir = "$cwd\build"
$bin_dir = "$build_dir\$output_dir\bin"

Write-Host "Executing windows/run.ps1" -ForegroundColor Green

Write-Host "Current working directory: $cwd"
Write-Host "Current application name: $app_name"
Write-Host "Current build directory: $build_dir"
Write-Host "Current bin directory: $bin_dir"

Write-Host "Running script for Windows desktop"

if (!(Test-Path "$bin_dir"))
{	
    Invoke-Expression -Command "$cwd\scripts\windows\build.ps1 -o=$output_dir" 
}

#-------------------------------------------------------------------------
if(Test-Path -Path "$bin_dir\$app_name" -PathType Leaf)
{
    if($args -eq $null -or $args.count -eq 0)
    {
        Write-Host "Start running application with no arguments ..."
        
        Start-Process $bin_dir\$app_name    
    }
    else
    {
        Write-Host "Start running application with arguments ..."

        Start-Process $bin_dir\$app_name -ArgumentList $args 
    }
}
else
{
    Write-Error "File does not exist: $bin_dir\$app_name" -ErrorAction Stop
}