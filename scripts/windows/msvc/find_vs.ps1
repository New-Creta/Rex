# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

$cwd = Get-Location

Write-Host "Loading windows/msvc/find_vs.ps1" -ForegroundColor Green
Write-Host "Current working directory: $cwd"

#-------------------------------------------------------------------------
Function find_vs_path()
{
    #
    # Find vswhere (installed with recent Visual Studio versions).
    #
    if ($vsWhere = Get-Command "vswhere.exe" -ErrorAction SilentlyContinue) 
    {
        $vsWhere = $vsWhere.Path
    }
    elseif (Test-Path "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe") 
    {
        $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    }
    else 
    {
        Write-Error "vswhere not found." -ErrorAction Stop
    }

    Write-Host "vswhere found at: $vsWhere"


    #
    # Get path to Visual Studio installation using vswhere.
    #
    $vsPath = &$vsWhere -latest -products * -version "16.1" `
        -requires Microsoft.Component.MSBuild `
        -property installationPath

    if ([string]::IsNullOrEmpty("$vsPath")) 
    {
        Write-Error "Failed to find Visual Studio 2019 installation." -ErrorAction Stop
    }

    Write-Host "Using Visual Studio installation at: ${vsPath}"

    return ${vsPath}
}