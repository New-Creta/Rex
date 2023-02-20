import os
import requests
import zipfile
import shutil

def __get_script_path():
    return os.path.dirname(os.path.realpath(__file__))

def fetch(url, cod, cwd):
    # get basename of the URL (a.k.a. the filename + extention we would like to download)
    url_basename = os.path.basename(url)

    # full output directory where we store downloaded repostiories
    parent_full_path = os.path.join(cwd, os.path.dirname(cod))
    desired_full_path = os.path.join(cwd, cod)

    # if we already doenloaded the repository
    # we do not need to download it again
    if os.path.exists(desired_full_path):
        print("Archive already present: " + desired_full_path)
        return 1

    # if the parent path of the ouput directory 
    # does not exist, we create it
    if not os.path.exists(parent_full_path):
        os.makedirs(parent_full_path)

    # # print the parent path of the output directory
    # print("Parent path of the output directory: " + parent_full_path)
    # # print the path where we would like to store our repository
    # print("Output directory for archive: " + desired_full_path)

    # request a download of the given URL
    if not os.path.exists(url_basename):
        response = requests.get(url)
        if response.status_code == requests.codes.ok:
            # write the downloaded file to disk
            open(url_basename, "wb").write(response.content)
        else:
            # bad request was made
            print("Bad request [" + str(response.status_code) + "] for given url: " + url)
            os.remove(desired_full_path)
            return 1
        
    # extract the zip file on disk
    # we cache the files within the directory before 
    # and after extraction, this gives us the ability
    # to examine the added files within the directory
    print("Extracting: " + url)
    
    # pre list directories
    # cached directories before we downloaded anything
    pre_list_dir = os.listdir(cwd)
    # print("pre-list-dir: " + " ".join(pre_list_dir))
    with zipfile.ZipFile(url_basename,"r") as zip_ref:
        zip_ref.extractall(cwd)

    # post list directories
    # directories after we downloaded the repository
    post_list_dir = os.listdir(cwd)
    # print("post-list-dir: " + " ".join(post_list_dir))

    print("Looking for added extracted directories ...")
    added_directory_names = []
    for post_dir in post_list_dir:
        count = pre_list_dir.count(post_dir)
        if count == 0:
            added_directory_names.append(post_dir)
    print("Found (" + str(len(added_directory_names)) + "): " + " ".join(added_directory_names))
    
    # store the actual scripts working directory
    script_cwd = __get_script_path()
    # # print the actual script directory
    # print("Actual scripts directory: " + script_cwd)

    # when only one directory was found we only need
    # to move it, when multiple file where found 
    # we create a new directory and move all files
    # into that directory
    if len(added_directory_names) == 1:
        shutil.move(os.path.join(cwd, added_directory_names[0]), desired_full_path)
    elif len(added_directory_names) > 1:
        os.mkdir(os.path.join(script_cwd, cod))
        for added_directory_name in added_directory_names:
            shutil.move(os.path.join(script_cwd, added_directory_name), desired_full_path)
    else:
        print("No directories where extracted.")
        os.remove(desired_full_path)
        return 1
    


    # remove the created zip file
    os.remove(url_basename)

    return 0