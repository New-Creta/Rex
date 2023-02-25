import os
import json
import requests
import zipfile
import shutil
from enum import Enum

class Host(Enum):
    UNKNOWN = 0
    GITLAB = 1
    GITHUB = 2

def __get_script_path():
    return os.path.dirname(os.path.realpath(__file__))
def __get_root_path():
    root = __get_script_path()
    root = os.path.join(root, os.path.join("..", ".."))

    return root

def __get_host(path):
    if "gitlab" in path:
        return Host.GITLAB
    elif "github" in path:
        return Host.GITHUB
    
    print("Unknown host!")
    return Host.UNKNOWN

def __build_gitlab_path(baseUrl, name, tag):
    url = os.path.join(baseUrl, "-")
    url = os.path.join(url, "archive")
    url = os.path.join(url, tag)
    url = os.path.join(url, name+"-"+tag+".zip")
    url = url.replace("\\", "/")
    return url
def __build_github_path(baseUrl, tag):
    url = os.path.join(baseUrl, "archive")
    url = os.path.join(url, "refs")
    url = os.path.join(url, "tags")
    url = os.path.join(url, tag+".zip")
    url = url.replace("\\", "/")
    return url
def __build_host_path(baseUrl, name, tag):
    host = __get_host(baseUrl)
    match host:
        case Host.GITHUB:
            return __build_github_path(baseUrl, tag)
        case Host.GITLAB:
            return __build_gitlab_path(baseUrl, name, tag)
        case Host.UNKNOWN:
            return ""

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

def __touch_externals_dir(externalsDir, tag):
    version = {
        "tag": tag
    }

    json_object = json.dumps(version, indent=4)

    # Writing to version.json
    with open(os.path.join(externalsDir, "version.json"), "w") as out:
        out.write(json_object)

def __install_external(external):
    cwd = os.getcwd()
    root = __get_root_path()

    external_url = external["url"]
    external_repository_name = external["repository_name"]
    external_directory_name = external["directory_name"]
    external_tag = external["tag"]
    external_store = external["storage"]
    external_store = external_store.replace("~", root)

    url = __build_host_path(external_url, external_repository_name, external_tag)

    # if the external is already present we need to check if we need to redownload anything
    should_download = False
    externals_dir = os.path.join(external_store, external_directory_name)
    if os.path.exists(externals_dir):
        print("External found: " + externals_dir + " validating version ...")
        version_file = os.path.join(externals_dir, "version.json")
        if os.path.exists(version_file):
            version_data = __load_json(version_file)           
            if version_data == None:
                print("Invalid version data found, redownloading external: " + url)
                should_download = True              
            if not version_data["tag"] == external_tag:
                should_download = True
            else:
                print("External: " + external_repository_name + " is up to date (" + external_repository_name + " " + external_tag + ")")
        else:
            print("We did not find: " + version_file)
            should_download = True

        # any data that was already available will be deleted 
        # the data will be out of date anyway when a download is triggered
        if should_download:
            shutil.rmtree(externals_dir)
    else:
        should_download = True

    if should_download:    
        added_directories = __download_external(url)       
        if len(added_directories) == 1:
            # move to output directory
            shutil.move(os.path.join(__get_script_path(), added_directories[0]), os.path.join(external_store, added_directories[0]))
            # change directory name
            os.chdir(external_store)
            os.rename(added_directories[0], external_directory_name)
            os.chdir(cwd)
        elif len(added_directories) > 1:
            # create output directory
            if not os.path.exists(externals_dir):
                os.makedirs(externals_dir)
            # move to output directory
            for added_directory in added_directories:
                shutil.move(os.path.join(__get_script_path(), added_directory), externals_dir)
        else:
            print("No directories where extracted.")
            return

        __touch_externals_dir(externals_dir, external_tag)   

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