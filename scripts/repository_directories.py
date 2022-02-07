from logging import error
import os

##-------------------------------------------------------------------------------
## GLOBALS

##-------------------------------------------------------------------------------
## A git directory is always named the following =>
git_directory_name = '.git'
externals_directory_name = "externals"
rex_stl_directory_name = "rex-stl"

##------------------------------------------------------------------------------
cached_git_directory = ""
cached_root_directory = ""

##-------------------------------------------------------------------------------
## PUBLIC FUNCTIONS

##-------------------------------------------------------------------------------
## Retrieve the git directory of this repository clone
def get_git_directory():
    global cached_git_directory
    if not __is_empty_string(cached_git_directory):
        return cached_git_directory

    print("Looking for git directory ... ")

    current_working_directory = os.path.normpath(os.getcwd())
    git_directory = __find_folder_in_current_or_above(git_directory_name, current_working_directory)

    print("Git directory found: " + git_directory)
    cached_git_directory = git_directory
    return git_directory
    
##-------------------------------------------------------------------------------
# Retrieve the root directory of this repository clone
def get_root_directory():
    global cached_root_directory
    if not __is_empty_string(cached_root_directory):
        return cached_root_directory

    print("Looking for root directory ... ( dependant on the git directory )")

    root_directory = os.path.normpath(os.path.join(get_git_directory(), os.pardir))

    print("Root directory found: " + root_directory)

    cached_root_directory = root_directory
    return root_directory

##-------------------------------------------------------------------------------
## Retrieve external directory
def get_externals_directory():
    root_dir = get_root_directory()
    externals_directory = __find_folder_in_current_or_above(externals_directory_name, root_dir)

    return externals_directory

##-------------------------------------------------------------------------------
## Retrieve rex-stl directory
def get_rex_stl_directory():
    externals_directory = get_externals_directory()
    rex_stl_directory = __find_folder_in_current_or_above(rex_stl_directory_name, externals_directory)

    return rex_stl_directory


##-------------------------------------------------------------------------------
## PRIVATE FUNCTIONS

##-------------------------------------------------------------------------------
## Find dirname in currentdir or above recursively
def __find_folder_in_current_or_above(dirname, currentdir):
    while_counter = 0
    max_while_counter = 100
    
    print("Looking for " + dirname)
    
    dirname_target = ""
    active_directory = currentdir

    while dirname_target == "" and while_counter < max_while_counter:
        ## Prevent infinite loop
        sub_folders = __get_subfolders(active_directory)
        while_counter += 1

        # for sf in sub_folders:
        #     print("\tdepth: " , while_counter , "found directories: " , os.path.normpath(sf))

        indices = [i for i, elem in enumerate(sub_folders) if elem.endswith(dirname)]
        if not indices:
            prev_active_directory = active_directory
            active_directory = os.path.join(active_directory, os.pardir)
            active_directory = os.path.normpath(active_directory)
            
            if active_directory == prev_active_directory:
                print("\t\tWe reached the root of our directory")
                error("\t\tCould not find directory " + dirname)
                break

            continue

        dirname_target = sub_folders[indices[0]]
        dirname_target = os.path.normpath(dirname_target)
    
    if dirname_target == "":
        error("\t\tCould not find directory " + dirname +". Max iterations reached(=100)")

    return dirname_target
##-------------------------------------------------------------------------------
## Get all subfolders inside the given directory
def __get_subfolders(dirname):
    return [f.path for f in os.scandir(dirname) if f.is_dir()]
##-------------------------------------------------------------------------------
## Check if a string is empty
def __is_empty_string(str):
    if len(str) == 0:
        return True
    return False