# -------------------------------
# Setup GIT
# Setup Externals
# 
# When git IS NOT installed no fetching will be done.
# 	In this case the externals need to be copied manually.
# When git IS installed CMake will expect that the the working directory is a git repository.
# -------------------------------
find_package(Git QUIET)

if(GIT_FOUND)
	if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
		option(REX_FETCH_SUBMODULE "Fetch Submodules" OFF)
		if(REX_FETCH_SUBMODULE)

			set(SUBMODULE_STATUS)

			# This will update all submodules
			# Log a message that we will fetch the submodules
			message(STATUS "Fetching submodules ...")

			# Check if a submodules needs to be updated
			execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --remote
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
							RESULT_VARIABLE GIT_SUBMOD_RESULT)
			# Query the status of a submodule
			execute_process(COMMAND ${GIT_EXECUTABLE} submodule status
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
							OUTPUT_VARIABLE SUBMODULE_STATUS)
				
			# Log a message with the status of the included submodules
			message(STATUS ${SUBMODULE_STATUS})

			if(NOT GIT_SUBMOD_RESULT EQUAL "0")
				message(WARNING "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, continuing the configuration ...")
			endif()
		endif()
	else()
	# Log a message when we are not a git repository
	message(FATAL_ERROR "${PROJECT_NAME} is not a git repository")
	endif()
else()
	# Log a message when the git package was not found
	message(FATAL_ERROR "Git package was not found, did you install GIT?")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/src/extern/CMakeLists.txt")
    message(WARNING "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

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