# -------------------------------
# Setup CMake minimum version
# -------------------------------
cmake_minimum_required(VERSION 3.8)

# -------------------------------
# Rex Version
# -------------------------------
SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 2)
SET(VERSION_PATCH 0)

# -------------------------------
# CMake dependencies
# -------------------------------
include(build\\cmake\\api_targets.txt)
include(build\\cmake\\build_targets.txt)

include(build\\cmake\\macro_helpers.txt)
include(build\\cmake\\macro_libraries.txt)
include(build\\cmake\\macro_applications.txt)
include(build\\cmake\\macro_install.txt)

message(STATUS "#--------------------- Starting project generation ---------------------")
# --------------------------------
# Setup engine specific properties
# --------------------------------
SET_PROPERTY(CACHE REX_BUILD_TARGET PROPERTY STRINGS ${BUILD_TARGETS})
SET_PROPERTY(CACHE REX_API_TARGET PROPERTY STRINGS ${API_TARGETS})

message(STATUS "Log Level: \t\t${REX_LOG_LEVEL}")
message(STATUS "Build Target: \t\t${REX_BUILD_TARGET} ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS "API Target: \t\t${REX_API_TARGET}")

# -------------------------------
# Setup the "Rex Engine" solution
# -------------------------------
project(rex_${LOWER_BUILD_TARGET}_${LOWER_API_TARGET} 	
		VERSION     "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
		LANGUAGES 	C;CXX)

# -------------------------------
# CMake project dependencies
# -------------------------------
include(build\\cmake\\directories.txt)

message(STATUS "CMake Version: \t\t${CMAKE_VERSION}")
message(STATUS "Generating project: \t${PROJECT_NAME} v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# --------------------------------
# Setup properties
# --------------------------------
# MSVC properties
# --------------------------------
if(MSVC)
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
endif()

# Debug postfix
SET (CMAKE_DEBUG_POSTFIX _d)

# --------------------------------
# Setup compiler features
# --------------------------------
SET (CMAKE_CXX_STANDARD 14)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)
SET (CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------
# Setup CMAKE output directories
# --------------------------------
# Debug
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 			"${OUTPUT_BINDIR}")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG 			"${OUTPUT_BINDIR}")
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 			"${OUTPUT_BINDIR}")
# Release configuration with minimum size
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL 		"${OUTPUT_BINDIR}")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL 		"${OUTPUT_BINDIR}")
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL 		"${OUTPUT_BINDIR}")
# Release configuration with debug information
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO 	"${OUTPUT_BINDIR}")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO 	"${OUTPUT_BINDIR}")
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO 	"${OUTPUT_BINDIR}")
# Release configuration
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 		"${OUTPUT_BINDIR}")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE 		"${OUTPUT_BINDIR}")
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE 		"${OUTPUT_BINDIR}")

message(STATUS "Output directory: \t\t${OUTPUT_BINDIR}")

# Add application and libraries
add_subdirectory(src)

message(STATUS "#--------------------- Finished project generation ---------------------")