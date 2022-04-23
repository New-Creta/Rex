# -----------------
# Directories       
# ----------------- 
SET (SRC_DIR source)
SET (EXTERNALS_DIR externals)
SET (BIN_DIR bin)
SET (INSTALL_DIR install)
SET (CONTENT_DIR content)

SET (OUTPUT_BINDIR "${PROJECT_BINARY_DIR}/${BIN_DIR}")
SET (CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/${INSTALL_DIR}" CACHE STRING "InstallPrefix" FORCE)

# -----------------
# VS Solution Folder       
# ----------------- 
SET (EXTERNALS_VS_SF 0_extern)
