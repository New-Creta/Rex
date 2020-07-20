# --------------------------
# Setup sources
# --------------------------
SET (CORE_LIBRARY_NAME rex_core)

message(STATUS "Generating ${CORE_LIBRARY_NAME}")

file(GLOB_RECURSE TARGET_INC    "${CURRENT_CORE_PATH}/include/*.h")
file(GLOB_RECURSE TARGET_RC     "${CURRENT_CORE_PATH}/resources/core.manifest" "resources/Core.rc" "resources/resource.h")
file(GLOB_RECURSE TARGET_SRC    "${CURRENT_CORE_PATH}/src/*.cpp")

GROUPSOURCES("include")
GROUPSOURCES("resources")
GROUPSOURCES("src")

# --------------------------
# Setup library
# --------------------------
# Compiler flag /MD
SETUP_MULTITHREADING_MD()

ADD_LIBRARY(${CORE_LIBRARY_NAME} SHARED
            ${TARGET_INC}
            ${TARGET_RC}
            ${TARGET_SRC}) 

SETUP_LIBRARY(${CORE_LIBRARY_NAME})

# --------------------------
# Target properties
# --------------------------
STRING(TOUPPER ${CORE_LIBRARY_NAME} UPPER_LIB_NAME)

set_target_properties( ${CORE_LIBRARY_NAME} PROPERTIES      VS_DEBUGGER_WORKING_DIRECTORY    "${OUTPUT_BINDIR}"             # Working directory
                                                            FOLDER                           "1. Core Libraries"            # Project filter location
                                                            DEFINE_SYMBOL                    "${UPPER_LIB_NAME}_LIB" )      # Default library symbol can be empty ( already set )

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${CORE_LIBRARY_NAME} PUBLIC     
                                                $<BUILD_INTERFACE:${CURRENT_CORE_PATH}/include>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CORE_LIBRARY_NAME}>)

# --------------------------
# Install.
# --------------------------
SETUP_INSTALL_DIRECTORIES(${CORE_LIBRARY_NAME} rex_core_targets)

INSTALL_DIRECTORY("${CURRENT_CORE_PATH}/${INCLUDE_DIR}/" ${CMAKE_INSTALL_INCLUDEDIR}/${CORE_LIBRARY_NAME})