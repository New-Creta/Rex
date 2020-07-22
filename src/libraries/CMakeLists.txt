# --------------------------------
# Setup diagnostics path
# --------------------------------
set(DIAGNOSTICS_FOLDER "diagnostics")

set(CURRENT_DIAGNOSTICS_PATH ${CMAKE_CURRENT_LIST_DIR}/${DIAGNOSTICS_FOLDER})

IF(NOT EXISTS ${CURRENT_DIAGNOSTICS_PATH} OR NOT IS_DIRECTORY "${CURRENT_DIAGNOSTICS_PATH}")
    file(MAKE_DIRECTORY ${CURRENT_DIAGNOSTICS_PATH})
    message(FATAL_ERROR "Directory, ${CURRENT_DIAGNOSTICS_PATH} does not exist, directory was generated but configuration will be stopped.")
ENDIF()
# --------------------------------
# Setup core path
# --------------------------------
set(CORE_FOLDER "core")

set(CURRENT_CORE_PATH ${CMAKE_CURRENT_LIST_DIR}/${CORE_FOLDER})

IF(NOT EXISTS ${CURRENT_CORE_PATH} OR NOT IS_DIRECTORY "${CURRENT_CORE_PATH}")
    file(MAKE_DIRECTORY ${CURRENT_CORE_PATH})
    message(FATAL_ERROR "Directory, ${CURRENT_CORE_PATH} does not exist, directory was generated but configuration will be stopped.")
ENDIF()

# --------------------------------
# Add subdirectories
# --------------------------------
add_subdirectory(diagnostics)
add_subdirectory(core)