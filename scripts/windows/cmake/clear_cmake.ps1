# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

$cwd = Get-Location

Write-Host "Loading windows/cmake/clear_cmake.ps1" -ForegroundColor Green
Write-Host "Current working directory: $cwd"

Function clear_cmake
{
    Param
    (
        [String]
        $outputDir
    )

    if((Test-Path "$cwd\build\$outputDir"))
    {
        Write-Host "Clearing CMake cache ... "
        Push-Location -Path build
            cmd.exe /c del /f /s /q CMakeFiles
            cmd.exe /c del /f /s /q CMakeCache.txt
        Pop-Location
    }
    else
    {
        Write-Error "Path not found: $cwd\build" -ErrorAction Stop
    }
}