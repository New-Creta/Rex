import os
import sys
import datetime
import timeit
import argparse

import repository_directories
import repository_helpers

##-------------------------------------------------------------------------------
## PRIVATE FUNCTIONS

##-------------------------------------------------------------------------------
# Create build directory if non existing
def __create_path_if_non_existing(path):
    if not os.path.exists(path):
        os.mkdir (path)   

##-------------------------------------------------------------------------------
## PROGRAM

##-------------------------------------------------------------------------------
## Main function of this program
if __name__ == '__main__':
    t = timeit.default_timer()
    
    print('\n')
    print('#--------------------- System information ------------------')
    print('System version: ' + sys.version)

    print('\n')
    print('#--------------------- Directory setup ---------------------')

    root_directory = repository_directories.get_root_directory()

    # Directory name for the shadow build
    shadow_build_directory_name = "VC2019_x64"

    # Generate path for the shadow build directory
    default_shadow_build_directory = os.path.join(root_directory, os.pardir)
    default_shadow_build_directory = os.path.join(default_shadow_build_directory, shadow_build_directory_name)
    default_shadow_build_directory = os.path.normpath(default_shadow_build_directory)

    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--output", help="The output path of our project file generation", default=default_shadow_build_directory)
    parser.add_argument("-g", "--generator", help="The build system generator", default="Visual Studio 16")
    parser.add_argument("-a", "--architecture", help="The platform name if supported by the generator", default="x64")
    parser.add_argument("-u", "--update_submodules", help="The platform name if supported by the generator", default=False)
    parser.add_argument("-at", "--api_target", help="The rendering API we would like to use", default="OPENGL")
    parser.add_argument("-bt", "--build_target", help="The platform we would like to use", default="WINDOWS")

    ## Check parse args
    args, unknown = parser.parse_known_args()

    shadow_build_directory = args.output
    __create_path_if_non_existing(shadow_build_directory)
    shadow_build_directory = os.path.join(shadow_build_directory, args.build_target)
    __create_path_if_non_existing(shadow_build_directory)
    shadow_build_directory = os.path.join(shadow_build_directory, args.api_target)
    __create_path_if_non_existing(shadow_build_directory)
    shadow_build_directory = os.path.normpath(shadow_build_directory)

    print('\n')
    if args.update_submodules:
        print('#--------------------- Load submodules ---------------------')

        git_executable_path = repository_helpers.get_git_executable()

        git_cmd = git_executable_path
        git_cmd = "\"%s\" %s" %(git_cmd, "submodule")
        git_cmd = "%s %s" %(git_cmd, "update")

        print("Starting git command: \"%s\"" %git_cmd)
        os.system(git_cmd)

        git_cmd = git_executable_path
        git_cmd = "\"%s\" %s" %(git_cmd, "submodule")
        git_cmd = "%s %s" %(git_cmd, "status")

        print("Starting git command: \"%s\"" %git_cmd)
        os.system(git_cmd)

        print('\n')

    print('#--------------------- CMake preparation -------------------')
    
    # Change directoy to our shadow build directory
    os.chdir(shadow_build_directory)

    print('Generating cmake command ...')
    cmake_cmd = "cmake -S\"%s\" -B\"%s\"" %(root_directory, shadow_build_directory)
    cmake_cmd = "%s %s" %(cmake_cmd, "-G\"" + args.generator + "\"")
    cmake_cmd = "%s %s" %(cmake_cmd, "-A\"" + args.architecture + "\"")
    cmake_cmd = "%s %s" %(cmake_cmd, "-DREX_API_TARGET=\"" + args.api_target + "\"")
    cmake_cmd = "%s %s" %(cmake_cmd, "-DREX_BUILD_TARGET=\"" + args.build_target + "\"")

    print("\t- source directory: \t%s" %root_directory)
    print("\t- build directory: \t%s" %shadow_build_directory)
    print("\t- generator: \t\t%s" %args.generator)
    print("\t- architecture: \t%s" %args.architecture)
    print("\t- api target: \t\t%s" %args.api_target)
    print("\t- build target: \t%s" %args.build_target)

    print("Starting cmake command: %s" % cmake_cmd)
    print('\n')

    # Execute cmake batch command
    os.system( cmake_cmd )
    print("Script run {time}".format(time=datetime.timedelta(seconds=timeit.default_timer() - t)))

    os.startfile(shadow_build_directory)

