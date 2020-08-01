# --------------------------
# Setup sources
# --------------------------
message(CHECK_START "Generating ${GRAPHICS_LIBRARY_NAME}")

file(GLOB_RECURSE TARGET_INC    "${GRAPHICS_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE TARGET_RC     "${GRAPHICS_RESOURCES_DIRECTORY}/graphics.manifest" "${GRAPHICS_RESOURCES_DIRECTORY}/Graphics.rc" "${GRAPHICS_RESOURCES_DIRECTORY}/resource.h")
file(GLOB_RECURSE TARGET_SRC    "${GRAPHICS_SOURCE_DIRECTORY}/*.cpp")

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

ADD_LIBRARY(${GRAPHICS_LIBRARY_NAME} SHARED
            ${TARGET_INC}
            ${TARGET_RC}
            ${TARGET_SRC}) 

SETUP_LIBRARY(${GRAPHICS_LIBRARY_NAME})

# --------------------------
# Configure definition file
# --------------------------
configure_file (
    "${GRAPHICS_INCLUDE_DIRECTORY}/rex_graphics_defines.h.in"
    "${GRAPHICS_INCLUDE_DIRECTORY}/rex_graphics_defines.h"
    @ONLY)

# --------------------------
# Target properties
# --------------------------
STRING(TOUPPER ${GRAPHICS_LIBRARY_NAME} UPPER_LIB_NAME)

set_target_properties( ${GRAPHICS_LIBRARY_NAME} PROPERTIES  VS_DEBUGGER_WORKING_DIRECTORY    "${OUTPUT_BINDIR}"             # Working directory
                                                            FOLDER                           "2. Platform Libraries"        # Project filter location
                                                            DEFINE_SYMBOL                    "${UPPER_LIB_NAME}_LIB" )      # Library symbol

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${GRAPHICS_LIBRARY_NAME} PUBLIC     
                                                $<BUILD_INTERFACE:${GRAPHICS_INCLUDE_DIRECTORY}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${GRAPHICS_LIBRARY_NAME}>)
# --------------------------
# Target link libraries.
# --------------------------
target_link_libraries(${GRAPHICS_LIBRARY_NAME} PUBLIC ${CORE_LIBRARY_NAME})

# --------------------------
# Rendering API
# --------------------------
SETUP_RENDERINGAPI(${GRAPHICS_LIBRARY_NAME})

# --------------------------
# Install.
# --------------------------
SET(EXPORT_TARGETS ${GRAPHICS_LIBRARY_NAME})
SET(EXPORT_TARGETS_NAME ${GRAPHICS_LIBRARY_NAME}_targets_name)

INSTAll_TARGETS(${EXPORT_TARGETS} ${EXPORT_TARGETS_NAME})
INSTALL_HEADERS(${GRAPHICS_INCLUDE_DIRECTORY} ${CMAKE_INSTALL_INCLUDEDIR}/${GRAPHICS_LIBRARY_NAME})

# --------------------------
# Precompiled Header
# --------------------------
add_precompiled_header(${GRAPHICS_LIBRARY_NAME} "${GRAPHICS_INCLUDE_DIRECTORY}/rex_graphics_pch.h" "${GRAPHICS_SOURCE_DIRECTORY}/rex_graphics_pch.cpp")

message(CHECK_PASS "Done")