# --------------------------
# Setup sources
# --------------------------
message(CHECK_START "Generating ${PLATFORM_LIBRARY_NAME}")

SET(TARGET_INC)
SET(TARGET_SRC)

file(GLOB TARGET_GLOBAL_INC             "${PLATFORM_INCLUDE_DIRECTORY}/*.h")
file(GLOB TARGET_CORE_INC               "${PLATFORM_INCLUDE_DIRECTORY}/core/*.h")
file(GLOB_RECURSE TARGET_EVENTS_INC     "${PLATFORM_INCLUDE_DIRECTORY}/events/*.h")
file(GLOB_RECURSE TARGET_API_INC        "${PLATFORM_INCLUDE_DIRECTORY}/api/${LOWER_API_TARGET}/*.h")

file(GLOB TARGET_RC                     "${PLATFORM_RESOURCES_DIRECTORY}/platform.manifest" "${PLATFORM_RESOURCES_DIRECTORY}/Platform.rc" "${PLATFORM_RESOURCES_DIRECTORY}/resource.h")

file(GLOB TARGET_GLOBAL_SRC             "${PLATFORM_SOURCE_DIRECTORY}/*.cpp")
file(GLOB TARGET_CORE_SRC               "${PLATFORM_SOURCE_DIRECTORY}/core/*.cpp")
file(GLOB_RECURSE TARGET_EVENTS_SRC     "${PLATFORM_SOURCE_DIRECTORY}/events/*.cpp")
file(GLOB_RECURSE TARGET_API_SRC        "${PLATFORM_SOURCE_DIRECTORY}/api/${LOWER_API_TARGET}/*.cpp")

LIST(APPEND TARGET_INC ${TARGET_GLOBAL_INC} ${TARGET_CORE_INC} ${TARGET_EVENTS_INC} ${TARGET_API_INC})
LIST(APPEND TARGET_SRC ${TARGET_GLOBAL_SRC} ${TARGET_CORE_SRC} ${TARGET_EVENTS_SRC} ${TARGET_API_SRC})

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

ADD_LIBRARY(${PLATFORM_LIBRARY_NAME} STATIC
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
                                                            DEFINE_SYMBOL                    "" )                           # Library symbol

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${PLATFORM_LIBRARY_NAME} PUBLIC     
                                                $<BUILD_INTERFACE:${PLATFORM_INCLUDE_DIRECTORY}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PLATFORM_LIBRARY_NAME}>)

target_include_directories(${PLATFORM_LIBRARY_NAME} PUBLIC ${GRAPHICS_INCLUDE_DIRECTORY})

# --------------------------
# Target link libraries.
# --------------------------
target_link_libraries(${PLATFORM_LIBRARY_NAME} PUBLIC ${CORE_LIBRARY_NAME})
target_link_libraries(${PLATFORM_LIBRARY_NAME} PUBLIC ${GRAPHICS_LIBRARY_NAME})
target_link_libraries(${PLATFORM_LIBRARY_NAME} PUBLIC imgui)

# --------------------------
# Rendering API
# --------------------------
SETUP_RENDERINGAPI(${PLATFORM_LIBRARY_NAME})

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