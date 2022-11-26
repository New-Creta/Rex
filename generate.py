import os
from pathlib import Path

def __find_sharpmake_in_env_path():
  envPath = os.environ["PATH"]
  paths = envPath.split(os.pathsep)

  for path in paths:
    if not os.path.exists(path):
      continue

    subFilesOrFolders = os.listdir(path)
    for fileOrFolder in subFilesOrFolders:
      absPath = os.path.join(path, fileOrFolder)
      if os.path.isfile(absPath):
        stem = Path(absPath).stem.lower()
        if stem == "sharpmake.application":
          return absPath

  return ''

def __find_sharpmake_path():
  path = __find_sharpmake_in_env_path()
  if len(path) > 0:
    return path
    
  sharpmakePathFromSetup = os.path.join("build", "tools", "sharpmake", "sharpmake.application.exe")
  if os.path.exists(sharpmakePathFromSetup):
    return sharpmakePathFromSetup

  return ''
    

def run():
  sharpmake_path = __find_sharpmake_path()
  
  if len(sharpmake_path) == 0:
    raise Exception("Failed to fine sharpmake path")
  
  os.system(f"{sharpmake_path} /sources(\"build/sharpmake/src/main.sharpmake.cs\")")
  return

if __name__ == "__main__":
  run()