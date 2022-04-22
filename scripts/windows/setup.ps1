# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

$cwd = Get-Location
$external_dir = "$cwd\externals"

Write-Host "Executing windows\setup.ps1" -ForegroundColor Green
Write-Host "Current working directory: $cwd"

Write-Host "Running script for Windows desktop"

Import-Module "$cwd\scripts\windows\utilities\download_repo.ps1"

if (!(Test-Path "$external_dir"))
{	
    # Check that we have a 'external' folder
	Write-Host "External directory was not found, creating ..."
	New-Item -ItemType Directory -Path "$external_dir"
}

Write-Host "External directory: $external_dir"

# download_repo $external_dir "target_folder-target_platform" "extracted_folder_name" "download_url"

try
{
    Set-ExecutionPolicy -ExecutionPolicy Unrestricted -Scope CurrentUser
}
catch
{
    Write-Host "Could not set execution policy for CurrentUser to \"Unrestricted\"." -ForegroundColor Red
    Write-Host $_
}

Write-Host "ExecutionPolicy for CurrentUser switched to Unrestriced"

try
{
    Set-ExecutionPolicy -ExecutionPolicy Unrestricted -Scope LocalMachine
}
catch
{
    Write-Host "Could not set execution policy for LocalMachine to \"Unrestricted\"." -ForegroundColor Red
    Write-Host $_
}

Write-Host "ExecutionPolicy for LocalMachine switched to Unrestriced"

Write-Host "Listing execution policies ..."
Get-ExecutionPolicy -List

Write-Host "Execution policies have been changed." -ForegroundColor Yellow
Write-Host "A restart might be required" -ForegroundColor Yellow

Write-Host "Setup script completed" -ForegroundColor Green