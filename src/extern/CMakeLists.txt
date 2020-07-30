# --------------------------------
# Setup spdlog path
# --------------------------------
set(SPDLOG_FOLDER "spdlog")

set(CURRENT_SPDLOG_PATH ${CMAKE_CURRENT_LIST_DIR}/${SPDLOG_FOLDER})

IF(NOT EXISTS ${CURRENT_SPDLOG_PATH} OR NOT IS_DIRECTORY "${CURRENT_SPDLOG_PATH}")
    file(MAKE_DIRECTORY ${CURRENT_SPDLOG_PATH})
    message(FATAL_ERROR "Directory, ${CURRENT_SPDLOG_PATH} does not exist, directory was generated but configuration will be stopped.")
ENDIF()

SET(SPDLOG_INCLUDE_DIRECTORY ${CURRENT_SPDLOG_PATH}/include)

SET(SPDLOG_LIBRARY_NAME spdlog)
SET(SPDLOG_INTERFACE_NAME ${SPDLOG_LIBRARY_NAME}_interface)

# --------------------------------
# Setup glew path
# --------------------------------
if(${REX_API_TARGET} STREQUAL ${OPENGL})
	set(GLEW_FOLDER "glew")

	set(CURRENT_GLEW_PATH ${CMAKE_CURRENT_LIST_DIR}/${GLEW_FOLDER})

	IF(NOT EXISTS ${CURRENT_GLEW_PATH} OR NOT IS_DIRECTORY "${CURRENT_GLEW_PATH}")
    	file(MAKE_DIRECTORY ${CURRENT_GLEW_PATH})
    	message(FATAL_ERROR "Directory, ${CURRENT_GLEW_PATH} does not exist, directory was generated but configuration will be stopped.")
	ENDIF()

	SET(GLEW_INCLUDE_DIRECTORY ${CURRENT_GLEW_PATH}/include)
	SET(GLEW_SOURCE_DIRECTORY ${CURRENT_GLEW_PATH}/src)

	SET(GLEW_LIBRARY_NAME glew)
endif()

# --------------------------------
# Add subdirectories
# --------------------------------
# -----SPDLOG---------------------
if(EXISTS "${PROJECT_SOURCE_DIR}/src/extern/${SPDLOG_FOLDER}")
	add_subdirectory(${SPDLOG_FOLDER})
else()
	message(WARNING "Could not find \"${SPDLOG_FOLDER}\"")
endif()
# -----GLEW-----------------------
if(${REX_API_TARGET} STREQUAL ${OPENGL})
	find_package(OpenGL REQUIRED)

	if(EXISTS "${PROJECT_SOURCE_DIR}/src/extern/${GLEW_FOLDER}")
		add_subdirectory(${GLEW_FOLDER})
	else()
		message(WARNING "Could not find \"${GLEW_FOLDER}\"")
	endif()
endif()