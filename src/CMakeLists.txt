# --------------------------------
# Add subdirectories
# --------------------------------
message(STATUS "#--------- Adding externals ---------")
add_subdirectory(extern)
message(STATUS "#--------- Adding libraries ---------")
add_subdirectory(libraries)
message(STATUS "#--------- Adding applications ------")
add_subdirectory(applications)