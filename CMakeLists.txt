# -------------------------------
# Setup CMake minimum version
# -------------------------------
cmake_minimum_required(VERSION 3.16)

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
message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "Generating project: ${PROJECT_NAME} v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

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

message(STATUS "Output directory: ${OUTPUT_BINDIR}")
message(STATUS "Rex Stl directory: ${REX_STL_DIR}")
message(STATUS "Project Source directory: ${PROJECT_SOURCE_DIR}")

# --------------------------------
# Add subdirectories
# --------------------------------
add_subdirectory(${EXTERNALS_DIR})
add_subdirectory(${SRC_DIR})

message(STATUS "#---------------------- Ending project generation ----------------------")	