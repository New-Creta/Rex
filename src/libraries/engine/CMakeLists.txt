# --------------------------
# Setup sources
# --------------------------
message(CHECK_START "Generating ${ENGINE_LIBRARY_NAME}")

file(GLOB_RECURSE TARGET_INC    "${ENGINE_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE TARGET_RC     "${ENGINE_RESOURCES_DIRECTORY}/engine.manifest" "${ENGINE_RESOURCES_DIRECTORY}/Engine.rc" "${ENGINE_RESOURCES_DIRECTORY}/resource.h")
file(GLOB_RECURSE TARGET_SRC    "${ENGINE_SOURCE_DIRECTORY}/*.cpp")

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

ADD_LIBRARY(${ENGINE_LIBRARY_NAME} SHARED
            ${TARGET_INC}
            ${TARGET_RC}
            ${TARGET_SRC}) 

SETUP_LIBRARY(${ENGINE_LIBRARY_NAME})

# --------------------------
# Target properties
# --------------------------
STRING(TOUPPER ${ENGINE_LIBRARY_NAME} UPPER_LIB_NAME)

set_target_properties( ${ENGINE_LIBRARY_NAME} PROPERTIES    VS_DEBUGGER_WORKING_DIRECTORY    "${OUTPUT_BINDIR}"             # Working directory
                                                            FOLDER                           "3. Application Libraries"     # Project filter location
                                                            DEFINE_SYMBOL                    "${UPPER_LIB_NAME}_LIB" )      # Library symbol

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC     
                                                $<BUILD_INTERFACE:${ENGINE_INCLUDE_DIRECTORY}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${ENGINE_LIBRARY_NAME}>)

target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC ${UTILITIES_INCLUDE_DIRECTORY})
target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC ${PLATFORM_INCLUDE_DIRECTORY})

# --------------------------
# Target link libraries.
# --------------------------
target_link_libraries(${ENGINE_LIBRARY_NAME} PUBLIC ${PLATFORM_LIBRARY_NAME})

# --------------------------
# Install.
# --------------------------
SET(EXPORT_TARGETS ${ENGINE_LIBRARY_NAME})
SET(EXPORT_TARGETS_NAME ${ENGINE_LIBRARY_NAME}_targets_name)

INSTAll_TARGETS(${EXPORT_TARGETS} ${EXPORT_TARGETS_NAME})
INSTALL_HEADERS(${ENGINE_INCLUDE_DIRECTORY} ${CMAKE_INSTALL_INCLUDEDIR}/${ENGINE_LIBRARY_NAME})

# --------------------------
# Precompiled Header
# --------------------------
add_precompiled_header(${ENGINE_LIBRARY_NAME} "${ENGINE_INCLUDE_DIRECTORY}/rex_engine_pch.h" "${ENGINE_SOURCE_DIRECTORY}/rex_engine_pch.cpp")

message(CHECK_PASS "Done")