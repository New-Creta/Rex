# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

$arguments = $args

$cwd=$arguments[0]
$output_dir=$arguments[1]
$content_dir=$arguments[2]

if([string]::IsNullOrEmpty($cwd))
{
	Write-Error "Empty working directory, pass the working directory for this script as an argument"
}

Write-Host "Loading windows/build-events/copy_content.ps1" -ForegroundColor Green
Write-Host "Current working directory: $cwd"

try
{
    Push-Location -Path "$output_dir"
        Copy-Item -Path "$content_dir" -Destination "$output_dir" -Recurse -Force
    Pop-Location
}
catch
{
    Write-Host "Could not copy item: $content_dir"
    Write-Host $_
}