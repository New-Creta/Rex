import platform
import os
import sys
import subprocess 

if __name__ == "__main__":
    system = platform.system()
    system = system.lower()

    root_directory = os.path.join(os.getcwd(), "..")

    if system == "windows":
    
        windows_build_script_location = os.path.join(root_directory, "_build_windows.bat")
        window_build_script_template = "Powershell.exe -executionpolicy remotesigned -File .\\scripts\\windows\\build.ps1 %*"

        windows_generate_script_location = os.path.join(root_directory, "_generate_windows.bat")
        window_generate_script_template = "Powershell.exe -executionpolicy remotesigned -File .\\scripts\\windows\\generate.ps1 %*"

        windows_run_script_location = os.path.join(root_directory, "_run_windows.bat")
        window_run_script_template = "Powershell.exe -executionpolicy remotesigned -File .\\scripts\\windows\\run.ps1 %*"

        windows_setup_script_location = os.path.join(root_directory, "_setup_windows.bat")
        window_setup_script_template = "Powershell.exe -executionpolicy remotesigned -File .\\scripts\\windows\\setup.ps1 %*"

        windows_build_script = open(windows_build_script_location, "w+")
        windows_build_script.write(window_build_script_template)

        windows_generate_script = open(windows_generate_script_location, "w+")
        windows_generate_script.write(window_generate_script_template)

        windows_run_script = open(windows_run_script_location, "w+")
        windows_run_script.write(window_run_script_template)

        windows_setup_script = open(windows_setup_script_location, "w+")
        windows_setup_script.write(window_setup_script_template)
    
    elif system == "darwin":
    
        print("Platform not implemented: " + platform.system())
    
    elif system == "linux":
    
        print("Platform not implemented: " + platform.system())
        
    else:
    
        print("Unsupported platform: " + platform.system())