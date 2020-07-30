# --------------------------------
# Add subdirectories
# --------------------------------
if(NOT REX_BUILD_TARGET)
	message(FATAL_ERROR "Platform was not specified, stopping cmake configuration")
endif()

add_subdirectory(${LOWER_BUILD_TARGET})