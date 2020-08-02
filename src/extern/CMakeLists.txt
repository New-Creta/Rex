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
# Setup glm path
# --------------------------------
set(GLM_FOLDER "glm")

set(CURRENT_GLM_PATH ${CMAKE_CURRENT_LIST_DIR}/${GLM_FOLDER})

IF(NOT EXISTS ${CURRENT_GLM_PATH} OR NOT IS_DIRECTORY "${CURRENT_GLM_PATH}")
    file(MAKE_DIRECTORY ${CURRENT_GLM_PATH})
    message(FATAL_ERROR "Directory, ${CURRENT_GLM_PATH} does not exist, directory was generated but configuration will be stopped.")
ENDIF()

SET(GLM_INCLUDE_DIRECTORY ${CURRENT_GLM_PATH})

SET(GLM_LIBRARY_NAME glm)
SET(GLM_INTERFACE_NAME ${GLM_LIBRARY_NAME}_interface)

SET(BUILD_STATIC_LIBS TRUE)

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
# Setup imgui path
# --------------------------------
set(IMGUI_FOLDER "imgui")

set(CURRENT_IMGUI_PATH ${CMAKE_CURRENT_LIST_DIR}/${IMGUI_FOLDER})

IF(NOT EXISTS ${CURRENT_IMGUI_PATH} OR NOT IS_DIRECTORY "${CURRENT_IMGUI_PATH}")
    file(MAKE_DIRECTORY ${CURRENT_IMGUI_PATH})
    message(FATAL_ERROR "Directory, ${CURRENT_IMGUI_PATH} does not exist, directory was generated but configuration will be stopped.")
ENDIF()

SET(IMGUI_INCLUDE_DIRECTORY ${CURRENT_IMGUI_PATH})

SET(IMGUI_LIBRARY_NAME imgui)

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
	if(EXISTS "${PROJECT_SOURCE_DIR}/src/extern/${GLEW_FOLDER}")
		add_subdirectory(${GLEW_FOLDER})
	else()
		message(WARNING "Could not find \"${GLEW_FOLDER}\"")
	endif()
endif()
# -----GLM------------------------
if(EXISTS "${PROJECT_SOURCE_DIR}/src/extern/${GLM_FOLDER}")
	add_subdirectory(${GLM_FOLDER})
else()
	message(WARNING "Could not find \"${GLM_FOLDER}\"")
endif()
# -----IMGUI----------------------
if(EXISTS "${PROJECT_SOURCE_DIR}/src/extern/${IMGUI_FOLDER}")
	add_subdirectory(${IMGUI_FOLDER})
else()
	message(WARNING "Could not find \"${IMGUI_FOLDER}\"")
endif()