# --------------------------------
# Add subdirectories
# --------------------------------
# -----SPDLOG---------------------
if(EXISTS "${PROJECT_SOURCE_DIR}/src/extern/spdlog")
	add_subdirectory(spdlog)
else()
	message(WARNING "Could not find \"SPDLOG\"")
endif()