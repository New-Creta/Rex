# --------------------------
# Setup sources
# --------------------------
message(CHECK_START "Generating ${PLATFORM_LIBRARY_NAME}")

file(GLOB_RECURSE TARGET_INC    "${PLATFORM_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE TARGET_RC     "${PLATFORM_RESOURCES_DIRECTORY}/platform.manifest" "${PLATFORM_RESOURCES_DIRECTORY}/Platform.rc" "${PLATFORM_RESOURCES_DIRECTORY}/resource.h")
file(GLOB_RECURSE TARGET_SRC    "${PLATFORM_SOURCE_DIRECTORY}/*.cpp")

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

ADD_LIBRARY(${PLATFORM_LIBRARY_NAME} SHARED
            ${TARGET_INC}
            ${TARGET_RC}
            ${TARGET_SRC}) 

SETUP_LIBRARY(${PLATFORM_LIBRARY_NAME})

# --------------------------
# Configure definition file
# --------------------------
configure_file (
    "${PLATFORM_INCLUDE_DIRECTORY}/rex_platform_defines.h.in"
    "${PLATFORM_INCLUDE_DIRECTORY}/rex_platform_defines.h"
    @ONLY)

# --------------------------
# Target properties
# --------------------------
STRING(TOUPPER ${PLATFORM_LIBRARY_NAME} UPPER_LIB_NAME)

set_target_properties( ${PLATFORM_LIBRARY_NAME} PROPERTIES  VS_DEBUGGER_WORKING_DIRECTORY    "${OUTPUT_BINDIR}"             # Working directory
                                                            FOLDER                           "2. Platform Libraries"        # Project filter location
                                                            DEFINE_SYMBOL                    "${UPPER_LIB_NAME}_LIB" )      # Library symbol

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${PLATFORM_LIBRARY_NAME} PUBLIC     
                                                $<BUILD_INTERFACE:${PLATFORM_INCLUDE_DIRECTORY}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PLATFORM_LIBRARY_NAME}>)

target_include_directories(${PLATFORM_LIBRARY_NAME} PUBLIC ${DIAGNOSTICS_INCLUDE_DIRECTORY})
target_include_directories(${PLATFORM_LIBRARY_NAME} PUBLIC ${CORE_INCLUDE_DIRECTORY})

# --------------------------
# Target link libraries.
# --------------------------
target_link_libraries(${PLATFORM_LIBRARY_NAME} PUBLIC ${DIAGNOSTICS_LIBRARY_NAME})
target_link_libraries(${PLATFORM_LIBRARY_NAME} PUBLIC ${CORE_LIBRARY_NAME})

# --------------------------
# Install.
# --------------------------
SET(EXPORT_TARGETS ${PLATFORM_LIBRARY_NAME})
SET(EXPORT_TARGETS_NAME ${PLATFORM_LIBRARY_NAME}_targets_name)

INSTAll_TARGETS(${EXPORT_TARGETS} ${EXPORT_TARGETS_NAME})
INSTALL_HEADERS(${PLATFORM_INCLUDE_DIRECTORY} ${CMAKE_INSTALL_INCLUDEDIR}/${PLATFORM_LIBRARY_NAME})

# --------------------------
# Precompiled Header
# --------------------------
add_precompiled_header(${PLATFORM_LIBRARY_NAME} "${PLATFORM_INCLUDE_DIRECTORY}/rex_platform_pch.h" "${PLATFORM_SOURCE_DIRECTORY}/rex_platform_pch.cpp")

message(CHECK_PASS "Done")