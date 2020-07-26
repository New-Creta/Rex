# --------------------------
# Setup sources
# --------------------------
message(STATUS "Generating ${CORE_LIBRARY_NAME}")

file(GLOB_RECURSE TARGET_INC    "${CORE_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE TARGET_RC     "${CORE_RESOURCES_DIRECTORY}/core.manifest" "${CORE_RESOURCES_DIRECTORY}/Core.rc" "${CORE_RESOURCES_DIRECTORY}/resource.h")
file(GLOB_RECURSE TARGET_SRC    "${CORE_SOURCE_DIRECTORY}/*.cpp")

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
                                                            DEFINE_SYMBOL                    "${UPPER_LIB_NAME}_LIB" )      # Library symbol

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${CORE_LIBRARY_NAME} PUBLIC     
                                                $<BUILD_INTERFACE:${CORE_INCLUDE_DIRECTORY}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CORE_LIBRARY_NAME}>)

target_include_directories(${CORE_LIBRARY_NAME} PUBLIC ${UTILITIES_INCLUDE_DIRECTORY})
target_include_directories(${CORE_LIBRARY_NAME} PUBLIC ${DIAGNOSTICS_INCLUDE_DIRECTORY})

# --------------------------
# Target link libraries.
# --------------------------
target_link_libraries(${CORE_LIBRARY_NAME} PUBLIC ${DIAGNOSTICS_LIBRARY_NAME})

# --------------------------
# Install.
# --------------------------
SET(EXPORT_TARGETS ${CORE_LIBRARY_NAME})
SET(EXPORT_TARGETS_NAME ${CORE_LIBRARY_NAME}_targets_name)

INSTAll_TARGETS(${EXPORT_TARGETS} ${EXPORT_TARGETS_NAME})
INSTALL_HEADERS(${CORE_INCLUDE_DIRECTORY} ${CMAKE_INSTALL_INCLUDEDIR}/${CORE_LIBRARY_NAME})

# --------------------------
# Precompiled Header
# --------------------------
add_precompiled_header(${CORE_LIBRARY_NAME} "${CORE_INCLUDE_DIRECTORY}/rex_core_pch.h" "${CORE_SOURCE_DIRECTORY}/rex_core_pch.cpp")

message(CHECK_PASS "Done")