import os

##-------------------------------------------------------------------------------
## GLOBALS

##-------------------------------------------------------------------------------
## A git directory is always named the following =>
git_directory_name = '.git'

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

    max_while_counter = 100
    while_counter = 0

    current_working_directory = os.path.normpath(os.getcwd())
    print("\tCurrent working directory: " + current_working_directory)
    current_working_drive = os.path.splitdrive(current_working_directory) 
    if current_working_drive[0] == None:
        print("\tOperating system does not support drive letters")
        return None

    git_directory = ""
    active_directory = current_working_directory

    directories = __get_subfolders(current_working_directory)
    while git_directory == "" and while_counter < max_while_counter:
        ## Prevent infinite loop
        while_counter += 1

        for d in directories:
            print("\tdepth: " , while_counter , "found directories: " , os.path.normpath(d))

        indices = [i for i, elem in enumerate(directories) if elem.endswith(git_directory_name)]
        if not indices:
            active_directory = os.path.join(active_directory, os.pardir)
            active_directory = os.path.normpath(active_directory)
            if active_directory == os.path.normpath(current_working_drive[0] + os.path.sep):
                print("\t\tWe reached the root of our directory")
                print("\t\tWe will default to our current directory")
                break
            directories = __get_subfolders(active_directory)
            continue

        git_directory = directories[indices[0]]
        git_directory = os.path.normpath(git_directory)

    # fail safe
    if git_directory == "":
        print("\tFAILSAFE - Could not find the git directory using: " + current_working_directory)
        git_directory = current_working_directory
        git_directory = os.path.normpath(git_directory)


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
## PRIVATE FUNCTIONS

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