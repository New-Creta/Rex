# --------------------------
# Setup sources
# --------------------------
message(CHECK_START "Generating ${DIAGNOSTICS_LIBRARY_NAME}")

file(GLOB_RECURSE TARGET_INC    "${DIAGNOSTICS_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE TARGET_RC     "${DIAGNOSTICS_RESOURCES_DIRECTORY}/diagnostics.manifest" "${DIAGNOSTICS_RESOURCES_DIRECTORY}/Diagnostics.rc" "${DIAGNOSTICS_RESOURCES_DIRECTORY}/resource.h")
file(GLOB_RECURSE TARGET_SRC    "${DIAGNOSTICS_SOURCE_DIRECTORY}/*.cpp")

if(MSVC)
    GROUPSOURCES("include")
    GROUPSOURCES("resources")
    GROUPSOURCES("src")
endif()

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
                                                $<BUILD_INTERFACE:${DIAGNOSTICS_INCLUDE_DIRECTORY}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${DIAGNOSTICS_LIBRARY_NAME}>)

# --------------------------
# Target link libraries.
# --------------------------
target_link_libraries(${DIAGNOSTICS_LIBRARY_NAME} PUBLIC spdlog_interface)

# --------------------------
# Install.
# --------------------------
SET(EXPORT_TARGETS ${DIAGNOSTICS_LIBRARY_NAME})
SET(EXPORT_TARGETS_NAME ${DIAGNOSTICS_LIBRARY_NAME}_targets_name)

INSTAll_TARGETS(${EXPORT_TARGETS} ${EXPORT_TARGETS_NAME})
INSTALL_HEADERS(${DIAGNOSTICS_INCLUDE_DIRECTORY} ${CMAKE_INSTALL_INCLUDEDIR}/${DIAGNOSTICS_LIBRARY_NAME})

# --------------------------
# Precompiled Header
# --------------------------
add_precompiled_header(${DIAGNOSTICS_LIBRARY_NAME} "${DIAGNSOTICS_INCLUDE_DIRECTORY}/rex_diagnostics_pch.h" "${DIAGNOSTICS_SOURCE_DIRECTORY}/rex_diagnostics_pch.cpp")

message(CHECK_PASS "Done")