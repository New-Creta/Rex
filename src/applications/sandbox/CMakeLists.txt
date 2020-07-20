# --------------------------
# Setup sources
# --------------------------
SET (APPLICATION_NAME sandbox)

message(STATUS "Generating ${APPLICATION_NAME}")

file(GLOB_RECURSE TARGET_INC    "${CURRENT_SANDBOX_PATH}/include/*.h")
file(GLOB_RECURSE TARGET_RC     "${CURRENT_SANDBOX_PATH}/resources/sandbox.manifest" "resources/Sandbox.rc" "resources/resource.h")
file(GLOB_RECURSE TARGET_SRC    "${CURRENT_SANDBOX_PATH}/src/*.cpp")

GROUPSOURCES("include")
GROUPSOURCES("resources")
GROUPSOURCES("src")

ADD_EXECUTABLE(${APPLICATION_NAME}
               ${TARGET_INC}   
               ${TARGET_RC}
               ${TARGET_SRC}) 

SETUP_APPLICATION(${APPLICATION_NAME})

# --------------------------
# Target properties
# --------------------------
set_target_properties(${APPLICATION_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY      "${OUTPUT_BINDIR}"      # Working directory
                                                     FOLDER                             "2. Applications"       # Project filter location
                                                     DEBUG_POSTFIX                      ${CMAKE_DEBUG_POSTFIX}  # Postfix for a debug application ( this is not set automatically like a library )
                                                     DEFINE_SYMBOL                      "" )                    # Default library symbol can be empty ( already set )

# --------------------------
# Target include directories.
# --------------------------
target_include_directories(${APPLICATION_NAME} PUBLIC     
                                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${APPLICATION_NAME}>)

# --------------------------
# Target link libraries
# --------------------------
target_link_libraries(${APPLICATION_NAME} PUBLIC rex_core)

# --------------------------
# Install.
# --------------------------
SETUP_INSTALL_DIRECTORIES(${APPLICATION_NAME} sandbox_targets)

INSTALL_DIRECTORY("${CURRENT_SANDBOX_PATH}/${INCLUDE_DIR}" ${CMAKE_INSTALL_INCLUDEDIR}/${APPLICATION_NAME})