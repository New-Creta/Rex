

# This file was automatically generated by "create_project.py"
# Command: generate_project(type project_name solution_folder project_folder pch_header_name = project_folder pch_src_name = project_folder)
# Example: generate_project(static CoreLibs 1_common core_libs)


# include path: ${CMAKE_SOURCE_DIR}/source/include/4_app_libraries/rex_application_core
# src path: ${CMAKE_SOURCE_DIR}/source/src/4_app_libraries/rex_application_core


# Project Name Project
# -------------------------
file(GLOB_RECURSE rex_application_core_LIBS_INC    ${CMAKE_SOURCE_DIR}/source/include/4_app_libraries/rex_application_core/*.h)
file(GLOB_RECURSE rex_application_core_LIBS_SRC    ${CMAKE_SOURCE_DIR}/source/src/4_app_libraries/rex_application_core/*.cpp)


# Create the project filters
GROUPSOURCES(${CMAKE_SOURCE_DIR}/source/include/4_app_libraries/rex_application_core include)
GROUPSOURCES(${CMAKE_SOURCE_DIR}/source/src/4_app_libraries/rex_application_core src)


# Create the project
add_library(rex_application_core STATIC ${rex_application_core_LIBS_INC} ${rex_application_core_LIBS_SRC})


# Set the include directories
target_include_directories(rex_application_core PUBLIC ${CMAKE_SOURCE_DIR}/source/include/4_app_libraries/rex_application_core)

# Set the link libraries
target_link_libraries(rex_application_core PUBLIC rex_windows)

# Set project properties
set_target_properties(rex_application_core PROPERTIES FOLDER                                         	4_app_libraries)   				# solution folder
set_target_properties(rex_application_core PROPERTIES DEFINE_SYMBOL                                  	"" )                     		# defines
IF(MSVC)
	set_target_properties(rex_application_core PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY                	${OUTPUT_BINDIR})        		# working directory
	set_target_properties(rex_application_core PROPERTIES ARCHIVE_OUTPUT_DIRECTORY 					  	${OUTPUT_BINDIR})				# output directory
    set_target_properties(rex_application_core PROPERTIES LIBRARY_OUTPUT_DIRECTORY 					  	${OUTPUT_BINDIR})				# output directory
    set_target_properties(rex_application_core PROPERTIES RUNTIME_OUTPUT_DIRECTORY 					  	${OUTPUT_BINDIR})				# output directory


	set_target_properties(rex_application_core PROPERTIES VS_GLOBAL_EnableUnitySupport                 	True)                    		# unit builds on visual studio
	set_target_properties(rex_application_core PROPERTIES VS_GLOBAL_IncludeInUnityFile                 	True)
	set_target_properties(rex_application_core PROPERTIES VS_GLOBAL_OrderInUnityFile                   	100)
	set_target_properties(rex_application_core PROPERTIES VS_GLOBAL_CombineFilesOnlyFromTheSameFolder  	false)
	set_target_properties(rex_application_core PROPERTIES VS_GLOBAL_MinFilesInUnityFile                	2)
	set_target_properties(rex_application_core PROPERTIES VS_GLOBAL_MaxFilesInUnityFile                	0)
	set_target_properties(rex_application_core PROPERTIES VS_GLOBAL_MinUnityFiles                      	1)
	set_target_properties(rex_application_core PROPERTIES VS_GLOBAL_UnityFilesDirectory                	.)
ENDIF()

# Compiler options
IF(MSVC)
	target_compile_options(rex_engine PRIVATE /W4 /WX /MP)
endif()

# Set precompiled header
add_precompiled_header(rex_application_core "rex_application_core.h" "rex_application_core.cpp")