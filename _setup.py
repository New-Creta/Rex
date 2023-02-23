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

def __get_script_path():
    return os.path.dirname(os.path.realpath(__file__))

def __install_externals():
  install_externals_path = os.path.join(__get_script_path(), os.path.join("build", os.path.join("scripts", "install_externals.py")))
  p = subprocess.Popen(["python.exe", install_externals_path], stdout=sys.stdout)
  p.communicate()

def __main():
  parser = argparse.ArgumentParser()
  args, unknown = parser.parse_known_args()

  # let's assume if "build" and "source"are found, we're in the root
  build_exists = os.path.exists("./build")
  source_exists = os.path.exists("./source")

  if not build_exists or not source_exists:
    print("Error: You're not running setup.py from the root directory. Please run this from the root directory and try again")
    return

  __install_externals()

  # now that rexpy and rexstd is installed, we can safely call the rest of the code
  arguments_to_pass_on = ""
  for arg in unknown:
    arguments_to_pass_on += f" {arg}"

  os.system(f"py build/scripts/rexpy/rexpy/setup.py{arguments_to_pass_on}")

if __name__ == "__main__":
  start_time = time.time()
  __main()
  print("Info: --- %s seconds (total) ---" % (time.time() - start_time))