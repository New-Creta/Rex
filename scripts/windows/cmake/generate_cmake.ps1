# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

$cwd = Get-Location

Write-Host "Loading windows/cmake/generate_cmake.ps1" -ForegroundColor Green
Write-Host "Current working directory: $cwd"

Function add_cmake_command
{
    Param
    (
        [String[]]
        $acceptValues,
        [String[]]
        $declineValues,

        [switch]
        $switchValue
    )

    $output = ""
    if($switchValue)
    {
        foreach($acceptvalue in $acceptValues)
        {
            $output = $output + $acceptvalue
        }
    }
    else
    {
        foreach($declinevalue in $declineValues)
        {
            $output = $output + $declinevalue
        }
    }

    return $output
}

Function generate_cmake
{
    Param
    (
        [String]
        $outputDir,
        [String]
        $externalDir,
        [switch]
        $skip_unity,
        [switch]
        $profiling
    )

    # Check for a build folder
    if(!(Test-Path "$cwd\build")) 
    {
        Write-Host "Setup build directory: $cwd\build"
        Push-Location -Path $cwd
            New-Item -ItemType Directory -Path "build" | Out-Null
        Pop-Location
    }

    # Check for a ouput folder
    if([string]::IsNullOrEmpty($outputDir))
    {
        $outputDir = "windows"
    }

    if(!(Test-Path "$cwd\build\$outputDir"))
    {
        Write-Host "Setup output directory: $cwd\build\$outputDir"
        Push-Location "$cwd\build"
            New-Item -ItemType Directory -Path "$outputDir" | Out-Null
        Pop-Location
    }

    $cmake_build_command = "--no-warn-unused-cli -H""$cwd"" -B""$cwd/build/$outputDir"" -G ""Visual Studio 16 2019"" -T host=x64 -A x64"

    $cmake_build_command += add_cmake_command " -DSBT_PROFILE=1" " -DSBT_PROFILE=0" -switchValue:$profiling
    $cmake_build_command += add_cmake_command " -DREX_UNITY_BUILD=0" " -DREX_UNITY_BUILD=1" -switchValue:$skip_unity

    $cmake_build_command += " -DREX_STL_DIR=$externalDir/rex-stl/source"

    Write-Host "Running command: cmake $cmake_build_command"
    $process = Start-Process cmake -ArgumentList $cmake_build_command -NoNewWindow -PassThru -Wait
    $exit_code = $process.ExitCode
    
    Write-Host "-----------------------------------"
    if($exit_code -eq 0)
    {
        Write-Host "CMake process run was successfull, Exit Code: $exit_code"
    }
    else
    {
        Write-Host "CMake process run failed, Exit Code: $exit_code"
    }
}