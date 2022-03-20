from http.server import executable
import os
import sys
import datetime
import timeit
import argparse

import repository_directories

##-------------------------------------------------------------------------------
## PRIVATE FUNCTIONS
# Create a relative path to use in CMAKE script
def __make_cmake_relative(path):
    root_directory = repository_directories.get_root_directory()
    return path.replace(root_directory, "${CMAKE_SOURCE_DIR}")

##-------------------------------------------------------------------------------
# Create build directory if non existing
def __create_path_if_non_existing(path):
    if not os.path.exists(path):
        os.makedirs(path)   

##-------------------------------------------------------------------------------
## ENTRY POINT

##-------------------------------------------------------------------------------
## Main function of this program
if __name__ == "__main__":
    t = timeit.default_timer()

    print("\n")
    print("#--------------------- System information ------------------")
    print("System version: " + sys.version)
    
    print("\n")
    print("#--------------------- Directory setup ---------------------")

    root_directory = repository_directories.get_root_directory()
    git_directory = repository_directories.get_git_directory()

    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--type", help="Type of project to be generated")
    parser.add_argument("-n", "--project_name", help="Name of the project to be generated")
    parser.add_argument("-s", "--solution_folder", help="Solution Folder of the project")
    parser.add_argument("-p", "--project_folder", help="Project Folder of the project")
    parser.add_argument("-pch", "--pch_header", help="Name of the header file of the precompiled header", default = "Not Specified")
    parser.add_argument("-pcpp", "--pch_source", help="Name of the source file of the precompiled header", default = "Not Specified")

    ## Check parse args
    args, unknown = parser.parse_known_args()

    if not args.type:
        raise Exception("Please setup the \"type (-t, --type)\" for the project you'd like to generate.")

    is_library = False

    static_library_type = "static"
    dynamic_library_type = "dynamic"
    executable_type = "executable"

    if args.type != static_library_type and args.type != dynamic_library_type and args.type != executable_type:
        raise Exception("Only \"static\" or \"dynamic\" are possible options for the \"type\" argument")

    if args.type == static_library_type:
        library_type = static_library_type.upper()
        is_library = True
    elif args.type == dynamic_library_type:
        library_type = dynamic_library_type.upper()
        is_library = True
    elif args.type == executable_type:
        library_type = executable_type.upper()
        is_library = False            
    else:
        raise Exception("Unreachable code!")

    print("given project type: " + library_type)

    if not args.project_name:
        raise Exception("Please setup the \"project_name (-n, --project_name)\" for the project you'd like to generate.")
    project_name = args.project_name

    print("given project name: " + project_name)

    if not args.solution_folder:
        raise Exception("Please setup the \"solution folder (-s, --solution_folder)\" for the project you'd like to generate.")
    solution_folder = args.solution_folder;
    if not args.project_folder:
        raise Exception("Please setup the \"project folder (-p, --project_folder)\" for the project you'd like to generate.")
    project_folder = args.project_folder;

    source_folder_name = "source"
    cmake_folder_name = "cmake"
    include_folder_name = "include"
    src_folder_name = "src"

    source_files_directory = os.path.join(root_directory, source_folder_name)
    cmake_files_directory = os.path.join(root_directory, cmake_folder_name)

    if not os.path.exists(source_files_directory):
        raise Exception("\"" + source_folder_name + "\" directory was not found.")
    if not os.path.exists(cmake_files_directory):
        raise Exception("\"" + cmake_folder_name + "\" directory was not found.")

    print("source directory: " + source_files_directory)
    print("cmake directory: " + cmake_files_directory)

    include_solution_folder = os.path.join(source_files_directory, os.path.join(include_folder_name, args.solution_folder))
    __create_path_if_non_existing(include_solution_folder)

    print("include solution folder: " + include_solution_folder)

    include_project_folder = os.path.join(include_solution_folder, args.project_folder) 
    __create_path_if_non_existing(include_project_folder)

    print("include project folder: " + include_project_folder)

    src_solution_folder = os.path.join(source_files_directory, os.path.join(src_folder_name, args.solution_folder))
    __create_path_if_non_existing(src_solution_folder)

    print("src solution folder: " + src_solution_folder)

    src_project_folder = os.path.join(src_solution_folder, args.project_folder)
    __create_path_if_non_existing(src_project_folder)

    print("include project folder: " + src_project_folder)

    pch_header_na = "Not Specified";
    pch_header_path = ""
    if args.pch_header == pch_header_na:
        pch_header_path = os.path.join(include_project_folder, project_folder)

    pch_source_na = "Not Specified";
    pch_source_path = ""
    if args.pch_source == pch_source_na:
        pch_source_path = os.path.join(src_project_folder, project_folder)

    print("pch include path: " + pch_source_path)
    print("pch src path: " + pch_source_path)

    print("GROUPSOURCES(" + os.path.join("..", os.path.join(source_folder_name, include_project_folder)) + " " + include_folder_name + ")")

    print(__make_cmake_relative(include_project_folder))

    template =      "" 

    template +=     "\n\n"
    template +=     "# This file was automatically generated by \"create_project.py\"\n"
    template +=     "# Command: generate_project(type project_name solution_folder project_folder pch_header_name = project_folder pch_src_name = project_folder)\n"
    template +=     "# Example: generate_project(static CoreLibs 1_common core_libs)\n"
    template +=     "\n\n"
    template +=     "# include path: "  + __make_cmake_relative(include_project_folder)    + "\n"
    template +=     "# src path: "      + __make_cmake_relative(src_project_folder)        + "\n"
    template +=     "\n\n"
    template +=     "# Project Name Project\n"
    template +=     "# -------------------------\n"
    template +=     "file(GLOB_RECURSE " + project_name + "_LIBS_INC    " + __make_cmake_relative(include_project_folder) + "\*.h)\n"
    template +=     "file(GLOB_RECURSE " + project_name + "_LIBS_SRC    " + __make_cmake_relative(src_project_folder)     + "\*.cpp)\n"
    template +=     "\n\n"
    template +=     "# Create the project filters\n"
    template +=     "GROUPSOURCES(" + __make_cmake_relative(include_project_folder) + " " + include_folder_name + ")\n"
    template +=     "GROUPSOURCES(" + __make_cmake_relative(src_project_folder)     + " " + src_folder_name + ")\n"
    template +=     "\n\n"
    template +=     "# Create the project\n"
    
    # library or executable
    if is_library:
        template +=     "add_library(" + project_name + " " + library_type + " ${" + project_name + "_LIBS_INC} ${" + project_name + "_LIBS_SRC})\n"
        template +=     "\n"
        template +=     "STRING(TOUPPER " + project_name + " UPPER_LIB_NAME)\n"
        template +=     "add_definitions(-D${UPPER_LIB_NAME}_LIB)\n"
    else:
        template +=     "add_executable(" + project_name + " " + " ${" + project_name + "_LIBS_INC} ${" + project_name + "_LIBS_SRC})\n"  
    
    template +=     "\n\n"
    template +=     "# Set the include directories\n"
    template +=     "target_include_directories(" + project_name + " PUBLIC " + __make_cmake_relative(include_project_folder)   + ")\n"
    template +=     "\n\n"
    template +=     "# Set project properties\n"
    template +=     "set_target_properties(" + project_name + " PROPERTIES FOLDER                                         " + solution_folder + ")   \t\t# solution folder\n"
    template +=     "set_target_properties(" + project_name + " PROPERTIES DEFINE_SYMBOL                                  \"\" )                     \t\t# defines\n"
    template +=     "IF(MSVC)\n"
    template +=     "\tset_property(" + project_name + " PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY                ${OUTPUT_BINDIR})        \t\t# working directory\n"
    template +=     "\tset_target_properties(" + project_name + " PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                     ${OUTPUT_BINDIR})        \t\t# output directory\n"
    template +=     "\tset_target_properties(" + project_name + " PROPERTIES LIBRARY_OUTPUT_DIRECTORY                     ${OUTPUT_BINDIR})        \t\t# output directory\n"
    template +=     "\tset_target_properties(" + project_name + " PROPERTIES RUNTIME_OUTPUT_DIRECTORY                     ${OUTPUT_BINDIR})        \t\t# output directory\n"
    template +=     "\n\n"
    template +=     "\t\tIF(REX_UNITY)\n"
    template +=     "\t\tset_target_properties(" + project_name + " PROPERTIES VS_GLOBAL_EnableUnitySupport                 True)                    \t\t# unit builds on visual studio\n"
    template +=     "\t\tset_target_properties(" + project_name + " PROPERTIES VS_GLOBAL_IncludeInUnityFile                 True)\n"
    template +=     "\t\tset_target_properties(" + project_name + " PROPERTIES VS_GLOBAL_OrderInUnityFile                   100)\n"
    template +=     "\t\tset_target_properties(" + project_name + " PROPERTIES VS_GLOBAL_CombineFilesOnlyFromTheSameFolder  false)\n"
    template +=     "\t\tset_target_properties(" + project_name + " PROPERTIES VS_GLOBAL_MinFilesInUnityFile                2)\n"
    template +=     "\t\tset_target_properties(" + project_name + " PROPERTIES VS_GLOBAL_MaxFilesInUnityFile                0)\n"
    template +=     "\t\tset_target_properties(" + project_name + " PROPERTIES VS_GLOBAL_MinUnityFiles                      1)\n"
    template +=     "\t\tset_target_properties(" + project_name + " PROPERTIES VS_GLOBAL_UnityFilesDirectory                .)\n"
    template +=     "\tENDIF()\n"
    template +=     "ENDIF()\n"
    template +=     "\n\n"
    template +=     "# Compiler options\n"
    template +=     "IF(MSVC)\n"
    template +=     "\ttarget_compile_options(" + project_name + " PRIVATE /W4 /WX /MP)\n"
    template +=     "ENDIF()\n"
    template +=     "\n\n"
    template +=     "# Set precompiled header\n"
    template +=     "add_precompiled_header(" + project_name + " \"" + project_folder + "_pch.h\" \"" + project_folder + "_pch.cpp\")"

    template = template.replace("\\", "/")

    cmake_project_file_to_write = template
    cmake_project_file_location = os.path.join(cmake_files_directory, project_name + ".cmake")

    if not os.path.exists(cmake_project_file_location):
        cmake_list_file_to_write = "\ninclude("+ __make_cmake_relative(cmake_project_file_location) +")"
        cmake_list_file_to_write = cmake_list_file_to_write.replace("\\", "/")
        cmake_list_file_location = os.path.join(source_files_directory, "CMakeLists.txt")
        cmake_list_file = open(cmake_list_file_location, "a")
        cmake_list_file.write(cmake_list_file_to_write)

    cmake_project_file = open(cmake_project_file_location, "w+")
    cmake_project_file.write(template)

    pch_header_file_to_write = "#pragma once"
    pch_header_file_to_write = pch_header_file_to_write.replace("\\", "/")
    pch_header_file_location = os.path.join(cmake_files_directory, pch_header_path + "_pch.h")
    pch_header_file = open(pch_header_file_location, "w+")
    pch_header_file.write(pch_header_file_to_write)

    pch_source_file_to_write = "#include \"" + project_folder + "_pch.h\""
    pch_source_file_to_write = pch_source_file_to_write.replace("\\", "/")
    pch_source_file_location = os.path.join(cmake_files_directory, pch_source_path + "_pch.cpp")
    pch_source_file = open(pch_source_file_location, "w+")
    pch_source_file.write(pch_source_file_to_write)

    print("Script took {time} to execute.".format(time=datetime.timedelta(seconds=timeit.default_timer() - t)))