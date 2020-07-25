# --------------------------------
# Setup sandbox path
# --------------------------------
set(SANDBOX_FOLDER "sandbox")

set(CURRENT_SANDBOX_PATH ${CMAKE_CURRENT_LIST_DIR}/${SANDBOX_FOLDER})

IF(NOT EXISTS ${CURRENT_SANDBOX_PATH} OR NOT IS_DIRECTORY "${CURRENT_SANDBOX_PATH}")
    file(MAKE_DIRECTORY ${CURRENT_SANDBOX_PATH})
    message(FATAL_ERROR "Directory, ${CURRENT_SANDBOX_PATH} does not exist, directory was generated but configuration will be stopped.")
ENDIF()

SET(SANDBOX_INCLUDE_DIRECTORY ${CURRENT_SANDBOX_PATH}/include)
SET(SANDBOX_RESOURCES_DIRECTORY ${CURRENT_SANDBOX_PATH}/resources)
SET(SANDBOX_SOURCE_DIRECTORY ${CURRENT_SANDBOX_PATH}/src)

SET (APPLICATION_NAME sandbox)

# --------------------------------
# Add subdirectories
# --------------------------------
add_subdirectory(${SANDBOX_FOLDER})