# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: setup.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

# This script is meant to get called to set up a workstation
# and prepare it for development with Rex Engine.
# This script is in essence a bootstrap.
# It'll download the specified version of rexpy (aka regis)
# and then hand over to the setup script located at _build/scripts/setup.py

import os
import argparse
import subprocess
import sys
import time
  
rexpy_version = "0.1.45"

def __intsall_regis():
  os.system(f"py -m pip uninstall --yes regis")
  os.system(f"py -m pip install --user \"regis=={rexpy_version}\"")

def _root_check():
  # let's assume if "build" and "source" are found, we're in the root
  build_exists = os.path.exists("./_build")
  source_exists = os.path.exists("./source")

  return build_exists and source_exists  

def __main():
  parser = argparse.ArgumentParser()
  args, unknown = parser.parse_known_args()

  # To avoid any issues, we need to be running from the root of Rex.
  if not _root_check():
    print("Error: You're not running setup.py from the root directory. Please run this from the root directory and try again")
    return

  # This script is the ONLY script that can run without rexpy (aka regis) installed.
  # All other python scripts in the rex pipeline expect rexpy to be installed.
  # that's why we install it here.
  __intsall_regis()

  # Get a list of all the arguments not known by this script and pass them
  # on to the setup script that'll do the actual setup of tools, libs, ..
  arguments_to_pass_on = " ".join(unknown)

  # now that regis is installed, we can safely call the rest of the code
  return os.system(f"py _build/scripts/setup.py{arguments_to_pass_on}")

if __name__ == "__main__":
  res = __main()
  exit(res)