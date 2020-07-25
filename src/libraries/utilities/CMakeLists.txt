# --------------------------
# Setup sources
# --------------------------
message(CHECK_START "Generating ${UTILITIES_LIBRARY_NAME}")

# --------------------------
# rex utilities target
# --------------------------
add_library(${UTILITIES_INTERFACE_NAME} INTERFACE)

target_include_directories(${UTILITIES_INTERFACE_NAME} INTERFACE     
                                                $<BUILD_INTERFACE:${UTILITIES_INCLUDE_DIRECTORY}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${UTILITIES_LIBRARY_NAME}>)

#---------------------------------------------------------------------------------------
# Install/export targets and files
#---------------------------------------------------------------------------------------
SET(EXPORT_TARGETS ${UTILITIES_INTERFACE_NAME})
SET(EXPORT_TARGETS_NAME ${UTILITIES_LIBRARY_NAME}_targets_name)

INSTALL_TARGETS(${EXPORT_TARGETS} ${EXPORT_TARGETS_NAME})
INSTALL_HEADERS(${UTILITIES_INCLUDE_DIRECTORY} ${CMAKE_INSTALL_INCLUDEDIR}/${UTILITIES_LIBRARY_NAME})

#---------------------------------------------------------------------------------------
# Custom target to integrate into the IDE
#---------------------------------------------------------------------------------------
STRING(TOUPPER ${UTILITIES_LIBRARY_NAME} UPPER_LIB_NAME)

file(GLOB_RECURSE TARGET_INC    "${UTILITIES_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE TARGET_RC     "${UTILITIES_RESOURCES_DIRECTORY}/utilities.manifest" "${UTILITIES_RESOURCES_DIRECTORY}/Utilities.rc" "${UTILITIES_RESOURCES_DIRECTORY}/resource.h")

if(MSVC)
    GROUPSOURCES("include")
    GROUPSOURCES("resources")
endif()

add_custom_target(${UTILITIES_LIBRARY_NAME} SOURCES ${TARGET_INC} ${TARGET_RC})

set_target_properties( ${UTILITIES_LIBRARY_NAME} PROPERTIES      VS_DEBUGGER_WORKING_DIRECTORY  "${OUTPUT_BINDIR}"             # Working directory
                                                                 FOLDER                         "1. Core Libraries"            # Project filter location
                                                                 DEFINE_SYMBOL                  "${UPPER_LIB_NAME}_LIB" )      # Default library symbol can be empty ( already set )

message(CHECK_PASS "Done")