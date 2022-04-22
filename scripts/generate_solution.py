import repository_directories
import os
import string
import argparse

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('-skip_unity', action='store_true')
    args, unknown = parser.parse_known_args()

    use_unity = not args.skip_unity
    use_unity_define = "1" if use_unity else "0"

    rex_stl_directory = repository_directories.get_rex_stl_directory()
    rex_stl_directory = rex_stl_directory.replace("\\", "/")    
    cmake_command = "cmake -Bbuild -DREX_STL_DIR=\"" + rex_stl_directory + "/source/\""+ " -DREX_UNITY=" + use_unity_define

    os.system(cmake_command)    
