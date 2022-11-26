import os
import copy

from pathlib import Path

RequiredTools = ["cl", "link", "clang", "g++", "ninja", "sharpmake.application"]

def verify_tools():
  print("Verifying tools")

  envPath = os.environ["PATH"]
  paths = envPath.split(os.pathsep)

  stemsToFind = copy.deepcopy(RequiredTools)

  for path in paths:
    if not os.path.exists(path):
      continue

    subFilesOrFolders = os.listdir(path)
    for fileOrFolder in subFilesOrFolders:
      absPath = os.path.join(path, fileOrFolder)
      if os.path.isfile(absPath):
        stem = Path(absPath).stem.lower()
        if (stem in stemsToFind):
          stemsToFind.remove(stem)

          if len(stemsToFind) == 0:
            print("All tools found")
            return True
  
  print(f"Tools that weren't found: {stemsToFind}")
  return False


def run():
  return verify_tools() == True
