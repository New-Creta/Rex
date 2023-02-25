# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: setup.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

import os
import argparse
import subprocess
import sys
import time

def __find_rexpy_install_location():
  proc = subprocess.Popen("pip show rexpy", stdout=subprocess.PIPE)
  while proc.poll() is None:
    output = proc.stdout.read().decode("utf-8")
    lines = output.split('\n')
    for line in lines:
      if "Location" in line:
        location = line.removeprefix("Location: ")
        return os.path.join(location, "rexpy")
      
  return ''
  
def __install_rexpy():
  # os.system("pip uninstall --yes rexpy")
  # os.system("pip install -i https://test.pypi.org/simple/ rexpy")
  # return __find_rexpy_install_location()
  return f'c:/Users/nickd/AppData/Local/Programs/Python/Python310/Lib/site-packages/rexpy'

def __main():
  parser = argparse.ArgumentParser()
  args, unknown = parser.parse_known_args()

  # let's assume if "build" and "source" are found, we're in the root
  build_exists = os.path.exists("./build")
  source_exists = os.path.exists("./source")

  if not build_exists or not source_exists:
    print("Error: You're not running setup.py from the root directory. Please run this from the root directory and try again")
    return

  install_dir = __install_rexpy()

  if install_dir == '':
    raise Exception(f"Failed to install rexpy")

  # now that rexpy is installed, we can safely call the rest of the code
  arguments_to_pass_on = ""
  for arg in unknown:
    arguments_to_pass_on += f" {arg}"

  os.system(f"py build/scripts/setup.py -rexpy={install_dir}{arguments_to_pass_on}")

if __name__ == "__main__":
  start_time = time.time()
  __main()
  print("Info: --- %s seconds (total) ---" % (time.time() - start_time))