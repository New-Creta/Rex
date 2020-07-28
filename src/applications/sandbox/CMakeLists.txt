# --------------------------
# Setup sources
# --------------------------
message(CHECK_START "Generating ${APPLICATION_NAME}")

file(GLOB_RECURSE TARGET_INC    "${CURRENT_SANDBOX_PATH}/include/*.h")
file(GLOB_RECURSE TARGET_RC     "${CURRENT_SANDBOX_PATH}/resources/sandbox.manifest" "resources/Sandbox.rc" "resources/resource.h")
file(GLOB_RECURSE TARGET_SRC    "${CURRENT_SANDBOX_PATH}/src/*.cpp")

if(MSVC)
    GROUPSOURCES("include")
    GROUPSOURCES("resources")
    GROUPSOURCES("src")
endif()

ADD_EXECUTABLE(${APPLICATION_NAME}
               ${TARGET_INC}   
               ${TARGET_RC}
               ${TARGET_SRC}) 

SETUP_APPLICATION(${APPLICATION_NAME})

# --------------------------
# Target properties
# --------------------------
set_target_properties(${APPLICATION_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY      "${OUTPUT_BINDIR}"      # Working directory
                                                     FOLDER                             "3. Applications"       # Project filter location
                                                     DEBUG_POSTFIX                      ${CMAKE_DEBUG_POSTFIX}  # Postfix for a debug application ( this is not set automatically like a library )
                                                     DEFINE_SYMBOL                      "" )                    # Default library symbol can be empty ( already set )

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${APPLICATION_NAME} PRIVATE     
                                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${APPLICATION_NAME}>)

# --------------------------
# Target link libraries
# --------------------------
target_link_libraries(${APPLICATION_NAME} PRIVATE rex_core)
target_link_libraries(${APPLICATION_NAME} PRIVATE rex_platform)

# --------------------------
# Install.
# --------------------------
SET(EXPORT_TARGETS ${APPLICATION_NAME})
SET(EXPORT_TARGETS_NAME ${APPLICATION_NAME}_targets_name)

INSTAll_TARGETS(${EXPORT_TARGETS} ${EXPORT_TARGETS_NAME})
INSTALL_HEADERS(${SANDBOX_INCLUDE_DIRECTORY} ${CMAKE_INSTALL_INCLUDEDIR}/${APPLICATION_NAME})

message(CHECK_PASS "Done")