# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

$cwd = Get-Location

Write-Host "Loading windows/msvc/build_msvc.ps1" -ForegroundColor Green
Write-Host "Current working directory: $cwd"

#-------------------------------------------------------------------------
Import-Module "$cwd\scripts\windows\msvc\find_vs.ps1"

#-------------------------------------------------------------------------
Function build_msvc
{
    Param
    (
        [String]
        $outputDir,
        [String]
        $solutionName
    )
    
    $vs_dir = find_vs_path

    if((Test-Path "$cwd\build\$outputDir"))
    {
        Write-Host "Performing build using MSVC ... "
        Push-Location -Path "$cwd\build\$outputDir"
            cmd.exe /c "${vs_dir}\MSBuild\Current\Bin\MSBuild.exe" "$solutionName.sln" /t:Build
        Pop-Location
    }
    else
    {
        Write-Error "Path not found: $cwd\build\$outputDir" -ErrorAction Stop
    }
}