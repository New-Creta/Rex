# --------------------------
# Setup sources
# --------------------------
SET (DIAGNOSTICS_LIBRARY_NAME rex_diagnostics)

message(STATUS "Generating ${DIAGNOSTICS_LIBRARY_NAME}")

file(GLOB_RECURSE TARGET_INC    "${CURRENT_DIAGNOSTICS_PATH}/include/*.h")
file(GLOB_RECURSE TARGET_SRC    "${CURRENT_DIAGNOSTICS_PATH}/src/*.cpp")

GROUPSOURCES("include")
GROUPSOURCES("src")

# --------------------------
# Setup library
# --------------------------
# Compiler flag /MD
SETUP_MULTITHREADING_MD()

ADD_LIBRARY(${DIAGNOSTICS_LIBRARY_NAME} SHARED
            ${TARGET_INC}
            ${TARGET_RC}
            ${TARGET_SRC}) 

SETUP_LIBRARY(${DIAGNOSTICS_LIBRARY_NAME})

# --------------------------
# Target properties
# --------------------------
STRING(TOUPPER ${DIAGNOSTICS_LIBRARY_NAME} UPPER_LIB_NAME)

set_target_properties( ${DIAGNOSTICS_LIBRARY_NAME} PROPERTIES   VS_DEBUGGER_WORKING_DIRECTORY    "${OUTPUT_BINDIR}"             # Working directory
                                                            	FOLDER                           "1. Core Libraries"            # Project filter location
                                                            	DEFINE_SYMBOL                    "${UPPER_LIB_NAME}_LIB" )      # Default library symbol can be empty ( already set )

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${DIAGNOSTICS_LIBRARY_NAME} PUBLIC     
                                                $<BUILD_INTERFACE:${CURRENT_DIAGNOSTICS_PATH}/include>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${DIAGNOSTICS_LIBRARY_NAME}>)

# --------------------------
# Target link libraries.
# --------------------------
target_link_libraries(${DIAGNOSTICS_LIBRARY_NAME} PUBLIC spdlog_interface)

# --------------------------
# Install.
# --------------------------
SETUP_INSTALL_DIRECTORIES(${DIAGNOSTICS_LIBRARY_NAME} rex_diagnostics_targets)

INSTALL_DIRECTORY("${CURRENT_DIAGNOSTICS_PATH}/${INCLUDE_DIR}/" ${CMAKE_INSTALL_INCLUDEDIR}/${DIAGNOSTICS_LIBRARY_NAME})