# -------------------------------
# Setup CMake minimum version
# -------------------------------
cmake_minimum_required(VERSION 3.16)

# -------------------------------
# Options
# -------------------------------
option(FETCH_EXTERNALS "Fetch externals" ON)

# -------------------------------
# Rex Version
# -------------------------------
SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 1)
SET(VERSION_PATCH 0)

# -------------------------------
# Configurations
# -------------------------------
SET(CONFIGURATION_DEBUG Debug)
SET(CONFIGURATION_RELWITHDEBINFO RelWithDebInfo)
SET(CONFIGURATION_MINSIZEREL Release)
SET(CONFIGURATION_RELEASE Release)

# -------------------------------
# Setup the "Rex Engine" solution
# -------------------------------
project(rex VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
				  LANGUAGES C;CXX)

message(STATUS "#--------------------- Starting project generation ---------------------")
message("CMake Version: \t\t${CMAKE_VERSION}")
message("Architecture: \t\t${CMAKE_GENERATOR_PLATFORM}")
message("Generating project: \t${PROJECT_NAME} v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# -------------------------------
# CMake dependencies
# -------------------------------
include(cmake\\precompiled_header.cmake)
include(cmake\\utilities.cmake)
include(cmake\\directories.cmake)

# --------------------------
# Setup global properties
# --------------------------
# Enable folder structure in the IDE
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Remove the always defined _WINDOWS flag
# CPP
string (REPLACE "/D_WINDOWS" "" CMAKE_CUSTOM_CPP_FLAGS ${CMAKE_CXX_FLAGS})
string (REPLACE "/DWIN32" "" CMAKE_CUSTOM_CPP_FLAGS ${CMAKE_CUSTOM_CPP_FLAGS}) 
SET(CMAKE_CXX_FLAGS "${CMAKE_CUSTOM_CPP_FLAGS}" CACHE STRING "" FORCE)
# C
string (REPLACE "/D_WINDOWS" "" CMAKE_CUSTOM_C_FLAGS ${CMAKE_C_FLAGS})
string (REPLACE "/DWIN32" "" CMAKE_CUSTOM_C_FLAGS ${CMAKE_CUSTOM_C_FLAGS}) 
SET(CMAKE_C_FLAGS "${CMAKE_CUSTOM_C_FLAGS}" CACHE STRING "" FORCE)

# Debug postfix
SET (CMAKE_DEBUG_POSTFIX _d)

# --------------------------------
# Setup compiler features
# --------------------------------
SET (CMAKE_CXX_STANDARD 20)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)
SET (CMAKE_CXX_EXTENSIONS OFF)

message("Output directory: \t\t${OUTPUT_BINDIR}")
message("Project Source directory: \t\t${PROJECT_SOURCE_DIR}")

# -------------------------------
# Check if we need to update our submodules
# -------------------------------
IF(FETCH_EXTERNALS)
    find_package(Git)
    IF(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        # Update submodules as needed
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT
                        OUTPUT_VARIABLE GIT_SUBMOD_OUTPUT)
        IF(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        ELSE()
            message(STATUS ${GIT_SUBMOD_OUTPUT})
        ENDIF()
    ENDIF()

    IF(NOT EXISTS "${PROJECT_SOURCE_DIR}/externals/CMakeLists.txt")
        message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    ENDIF()
ENDIF()

# --------------------------------
# Add subdirectories
# --------------------------------
add_subdirectory(${EXTERNALS_DIR})
add_subdirectory(${SRC_DIR})

message(STATUS "#---------------------- Ending project generation ----------------------")	