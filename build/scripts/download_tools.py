import requests
import os
import zipfile 
import shutil
import threading
import loading_anim

targetFolder = "build"
zipDownloads = os.path.join(targetFolder, "zips")

def download_file(url):
  filename = os.path.basename(url)
  filePath = os.path.join(zipDownloads, filename)
  
  if not os.path.exists(filePath):
    response = requests.get(url)
    open(filePath, "wb").write(response.content)
  
def make_zip_download_path():
  if not os.path.exists(zipDownloads):
    os.mkdir(zipDownloads)

def download_tools_archive():
  loading_anim.start_animation()
  print("Downloading tools archive")

  threads = []

  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.001")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.002")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.003")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.004")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.005")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.006")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.007")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.008")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.009")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.010")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.011")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.012")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.013")))
  threads.append(__launch_download_thread(("https://github.com/RisingLiberty/RegisZip/raw/main/data/Tools.zip.014")))

  for thread in threads:
    thread.join()

  loading_anim.stop_animation()
  print("Downloading tools archive - Done")

def unzip_tools():
  print("Unzipping files")
  loading_anim.start_animation()

  zipPath = zipDownloads
  toolsZipFile = "tools.zip"
  zips = os.listdir(zipPath)
  for zipName in zips:
      with open(os.path.join(zipPath, toolsZipFile), "ab") as f:
          with open(os.path.join(zipPath, zipName), "rb") as z:
              f.write(z.read())

  with zipfile.ZipFile(os.path.join(zipPath, toolsZipFile), "r") as zipObj:
      zipObj.extractall(os.path.join(targetFolder, "tools"))

  loading_anim.stop_animation()

  print(f"tools unzipped to {os.path.basename(os.curdir)}")
    
  print("Unzipping files - Done")

def delete_tmp_folders():
  shutil.rmtree(zipDownloads)

def __launch_download_thread(url):
    thread = threading.Thread(target=download_file, args=(url,))
    thread.start()
    return thread

def run():
  print("Downloading tools..")

  make_zip_download_path()
  download_tools_archive()
  unzip_tools()
  delete_tmp_folders()
