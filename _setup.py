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
  
rexpy_version = "0.1.38"

def __intsall_regis():
  os.system(f"py -m pip uninstall --yes regis")
  os.system(f"py -m pip install --user \"regis=={rexpy_version}\"")
  
def __main():
  parser = argparse.ArgumentParser()
  args, unknown = parser.parse_known_args()

  # let's assume if "build" and "source" are found, we're in the root
  build_exists = os.path.exists("./_build")
  source_exists = os.path.exists("./source")

  if not build_exists or not source_exists:
    print("Error: You're not running setup.py from the root directory. Please run this from the root directory and try again")
    return

  __intsall_regis()

  # now that regis is installed, we can safely call the rest of the code
  arguments_to_pass_on = ""
  for arg in unknown:
    arguments_to_pass_on += f" {arg}"

  return os.system(f"py _build/scripts/setup.py{arguments_to_pass_on}")

if __name__ == "__main__":
  res = __main()
  exit(res)