

# This file was automatically generated by "create_project.py"
# Command: generate_project(type project_name solution_folder project_folder pch_header_name = project_folder pch_src_name = project_folder)
# Example: generate_project(static CoreLibs 1_common core_libs)


# include path: ${CMAKE_SOURCE_DIR}/source/include/6_tools/regina
# src path: ${CMAKE_SOURCE_DIR}/source/src/6_tools/regina


# Project Name Project
# -------------------------
file(GLOB_RECURSE Regina_LIBS_INC    ${CMAKE_SOURCE_DIR}/source/include/6_tools/regina/*.h)
file(GLOB_RECURSE Regina_LIBS_SRC    ${CMAKE_SOURCE_DIR}/source/src/6_tools/regina/*.cpp)


# Create the project filters
GROUPSOURCES(${CMAKE_SOURCE_DIR}/source/include/6_tools/regina include)
GROUPSOURCES(${CMAKE_SOURCE_DIR}/source/src/6_tools/regina src)


# Create the project
add_library(Regina STATIC ${Regina_LIBS_INC} ${Regina_LIBS_SRC})


# Set the include directories
target_include_directories(Regina PUBLIC ${CMAKE_SOURCE_DIR}/source/include/6_tools/regina)


# Set project properties
set_target_properties(Regina PROPERTIES FOLDER                                         6_tools)   		# solution folder
set_target_properties(Regina PROPERTIES DEFINE_SYMBOL                                  "" )                     		# defines
IF(MSVC)
	set_target_properties(Regina PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY                ${OUTPUT_BINDIR})        		# working directory


	set_target_properties(Regina PROPERTIES VS_GLOBAL_EnableUnitySupport                 True)                    		# unit builds on visual studio
	set_target_properties(Regina PROPERTIES VS_GLOBAL_IncludeInUnityFile                 True)
	set_target_properties(Regina PROPERTIES VS_GLOBAL_OrderInUnityFile                   100)
	set_target_properties(Regina PROPERTIES VS_GLOBAL_CombineFilesOnlyFromTheSameFolder  false)
	set_target_properties(Regina PROPERTIES VS_GLOBAL_MinFilesInUnityFile                2)
	set_target_properties(Regina PROPERTIES VS_GLOBAL_MaxFilesInUnityFile                0)
	set_target_properties(Regina PROPERTIES VS_GLOBAL_MinUnityFiles                      1)
	set_target_properties(Regina PROPERTIES VS_GLOBAL_UnityFilesDirectory                .)
ENDIF()


# Compiler options
target_compile_options(Regina PRIVATE /W4 /WX /MP)


# Set precompiled header
add_precompiled_header(Regina "regina_pch.h" "regina_pch.cpp")