import os
import json
import requests
import zipfile
import shutil

def __get_script_path():
    return os.path.dirname(os.path.realpath(__file__))
def __get_root_path():
    root = __get_script_path()
    root = os.path.join(root, os.path.join("..", ".."))

    return root

def __load_json(path):
  if not os.path.exists(path):
    print("Failed to load json, file does not exist: " + path)
    return None

  f = open(path)
  data = json.load(f)
  return data

def __load_externals_required():
    root = __get_root_path()

    json_blob = __load_json(os.path.join(root, "build", "config", "required_externals.json"))
    if json_blob == None:
        print("Loaded json blob is None, stopping json parse")
        return []

    externals_required = []
    for object in json_blob:
        externals_required.append(json_blob[object])

    return externals_required

def __download_external(url):
    # get basename of the URL (a.k.a. the filename + extention we would like to download)
    url_basename = os.path.basename(url)

    # request a download of the given URL
    if not os.path.exists(url_basename):
        response = requests.get(url)
        if response.status_code == requests.codes.ok:
            # write the downloaded file to disk
            open(url_basename, "wb").write(response.content)
        else:
            # bad request was made
            print("Bad request [" + str(response.status_code) + "] for given url: " + url)
            return 1
        
    # extract the zip file on disk
    # we cache the files within the directory before 
    # and after extraction, this gives us the ability
    # to examine the added files within the directory
    print("Extracting: " + url)
    
    # pre list directories
    # cached directories before we downloaded anything
    pre_list_dir = os.listdir(__get_script_path())
    # print("pre-list-dir: " + " ".join(pre_list_dir))
    with zipfile.ZipFile(url_basename,"r") as zip_ref:
        zip_ref.extractall(__get_script_path())

    # post list directories
    # directories after we downloaded the repository
    post_list_dir = os.listdir(__get_script_path())
    # print("post-list-dir: " + " ".join(post_list_dir))

    print("Looking for added extracted directories ...")
    added_directory_names = []
    for post_dir in post_list_dir:
        count = pre_list_dir.count(post_dir)
        if count == 0:
            added_directory_names.append(post_dir)
    print("Found (" + str(len(added_directory_names)) + "): " + " ".join(added_directory_names))

    # remove the created zip file
    os.remove(url_basename)

    return added_directory_names

def __install_external(external):
    root = __get_root_path()

    external_name = external["name"]
    external_url = external["url"]
    external_tag = external["tag"]
    
    url = os.path.join(external_url, "-")
    url = os.path.join(url, "archive")
    url = os.path.join(url, external_tag)
    url = os.path.join(url, external_name+"-"+external_tag+".zip")
    url = url.replace("\\", "/")
    
    added_directories = __download_external(url)
    
    external_store = external["storage"]
    external_store = external_store.replace("~", root)
    
    if len(added_directories) == 1:
        # move to output directory
        shutil.move(os.path.join(__get_script_path(), added_directories[0]), external_store)
        # change directory name
        cwd = os.getcwd()
        os.chdir(external_store)
        os.rename(added_directories[0], external_name)
        os.chdir(cwd)
    elif len(added_directories) > 1:
        # create output directory
        output_directory = os.path.join(external_store, external_name)
        os.makedirs(output_directory)
        # move to output directory
        for added_directory in added_directories:
            shutil.move(os.path.join(__get_script_path(), added_directory), output_directory)
    else:
        print("No directories where extracted.")
        return   

def __main():
    print("Start installing externals ...")

    externals_required = __load_externals_required()
    if externals_required == None:
        print("Required externals is None, exiting ...")
        return

    for external in externals_required:
        __install_external(external)    

if __name__ == "__main__":
    __main()