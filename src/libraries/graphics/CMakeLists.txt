# --------------------------------
# Add subdirectories
# --------------------------------
if(NOT REX_API_TARGET)
	message(FATAL_ERROR "API was not specified, stopping cmake configuration")
endif()

add_subdirectory(${LOWER_API_TARGET})