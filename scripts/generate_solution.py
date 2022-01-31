import repository_directories
import os
import string

if __name__ == "__main__":
    rex_stl_directory = repository_directories.get_rex_stl_directory()
    rex_stl_directory = rex_stl_directory.replace("\\", "/")    
    cmake_command = "cmake -Bbuild -DREX_STL_DIR=\"" + rex_stl_directory + "/source/\""
    os.system(cmake_command)    
