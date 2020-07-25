# --------------------------------
# Setup diagnostics path
# --------------------------------
set(DIAGNOSTICS_FOLDER "diagnostics")

set(CURRENT_DIAGNOSTICS_PATH ${CMAKE_CURRENT_LIST_DIR}/${DIAGNOSTICS_FOLDER})

IF(NOT EXISTS ${CURRENT_DIAGNOSTICS_PATH} OR NOT IS_DIRECTORY "${CURRENT_DIAGNOSTICS_PATH}")
    file(MAKE_DIRECTORY ${CURRENT_DIAGNOSTICS_PATH})
    message(FATAL_ERROR "Directory, ${CURRENT_DIAGNOSTICS_PATH} does not exist, directory was generated but configuration will be stopped.")
ENDIF()

SET(DIAGNOSTICS_INCLUDE_DIRECTORY ${CURRENT_DIAGNOSTICS_PATH}/include)
SET(DIAGNOSTICS_RESOURCES_DIRECTORY ${CURRENT_DIAGNOSTICS_PATH}/resources)
SET(DIAGNOSTICS_SOURCE_DIRECTORY ${CURRENT_DIAGNOSTICS_PATH}/src)

SET (DIAGNOSTICS_LIBRARY_NAME rex_diagnostics)

# --------------------------------
# Setup core path
# --------------------------------
set(CORE_FOLDER "core")

set(CURRENT_CORE_PATH ${CMAKE_CURRENT_LIST_DIR}/${CORE_FOLDER})

IF(NOT EXISTS ${CURRENT_CORE_PATH} OR NOT IS_DIRECTORY "${CURRENT_CORE_PATH}")
    file(MAKE_DIRECTORY ${CURRENT_CORE_PATH})
    message(FATAL_ERROR "Directory, ${CURRENT_CORE_PATH} does not exist, directory was generated but configuration will be stopped.")
ENDIF()

SET(CORE_INCLUDE_DIRECTORY ${CURRENT_CORE_PATH}/include)
SET(CORE_RESOURCES_DIRECTORY ${CURRENT_CORE_PATH}/resources)
SET(CORE_SOURCE_DIRECTORY ${CURRENT_CORE_PATH}/src)

SET (CORE_LIBRARY_NAME rex_core)

# --------------------------------
# Add subdirectories
# --------------------------------
add_subdirectory(${DIAGNOSTICS_FOLDER})
add_subdirectory(${UTILITIES_FOLDER})
add_subdirectory(${CORE_FOLDER})