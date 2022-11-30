import os
from pathlib import Path

def env_paths():
  envPath = os.environ["PATH"]
  paths = envPath.split(os.pathsep)
  return paths

def find_file_in_folder(file, path : str):
  fileToFind = file.lower()
  subFilesOrFolders = os.listdir(path)
  for fileOrFolder in subFilesOrFolders:
    absPath = os.path.join(path, fileOrFolder)
    if os.path.isfile(absPath):
      file_name = Path(absPath).name.lower()
      if file_name == fileToFind:
        return absPath
  
  return ''

def find_file_in_paths(file, directories : list[str]):
  for path in directories:
    if not os.path.exists(path):
      continue

    result = find_file_in_folder(file, path)
    if result != '':
      return result

  return ''

def find_in_parent(path, toFind):
  curr_path = path

  while toFind not in os.listdir(curr_path):
    if Path(curr_path).parent == curr_path:
      print(f"{toFind} not found in parents of {path}")
      return ''

    curr_path = Path(curr_path).parent

  return curr_path

def find_root():
  res = find_in_parent(os.getcwd(), "build")
  if (res == ''):
    print(f"Error: root not found")

  return res

def is_windows():
  return os.name == 'nt'