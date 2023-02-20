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
import pkg_resources
import shutil
import subprocess
import sys

def is_rexpy_installed():
  return os.path.exists(os.path.join(os.getcwd(), "rexpy"))
def is_rexstd_installed():
  return os.path.exists(os.path.join(os.getcwd(), os.path.join("..", os.path.join("..", os.path.join("source", os.path.join("0_thirdparty", "rex_std"))))))

def install_rexpy(forceInstall):
  # first set our working directory to rexpy.
  # this way the repository will be downloaded in {root}/build/scripts/rexpy
  # this keeps the root clean.

  # first let's make sure we're in the root directory
  # we don't have access to rexpy yet, we'll have to manually
  # check if we're in the root.

  # let's assume if "build" and "source"are found, we're in the root
  build_exists = os.path.exists("./build")
  source_exists = os.path.exists("./source")

  if not build_exists or not source_exists:
    print("Error: You're not running setup.py from the root directory. Please run this from the root directory and try again")
    return
  
  # rexpy is located in build/scripts
  cwd = os.getcwd()
  new_wd = os.path.join(cwd, "build", "scripts")
  os.chdir(new_wd)

  if forceInstall == False and is_rexpy_installed():
    print(f"rexpy is already installed - skipping install")
    return
  elif forceInstall == True:
    if is_rexpy_installed():
      print(f"remove the already install rexpy")
      shutil.rmtree("rexpy")

  # # now run the install script.
  p = subprocess.Popen(["python.exe", os.path.join(new_wd, 'fetch_rexpy.py')], stdout=sys.stdout)
  p.communicate()

  # reset the working directory
  os.chdir(cwd)

def install_rexstd(forceInstall):
  # first set our working directory to rexstd.
  # this way the repository will be downloaded in {root}/source/0_thirdparty/rexstd
  # this keeps the root clean.

  # first let's make sure we're in the root directory
  # we don't have access to rexstd yet, we'll have to manually
  # check if we're in the root.

  # let's assume if "build" and "source"are found, we're in the root
  build_exists = os.path.exists("./build")
  source_exists = os.path.exists("./source")

  if not build_exists or not source_exists:
    print("Error: You're not running setup.py from the root directory. Please run this from the root directory and try again")
    return

  # rexstd is located in build/scripts
  cwd = os.getcwd()
  new_wd = os.path.join(cwd, "build", "scripts")
  os.chdir(new_wd)

  if forceInstall == False and is_rexstd_installed():
    print(f"rexstd is already installed - skipping install")
    return
  elif forceInstall == True:
    if is_rexstd_installed():
      print(f"remove the already install rexstd")
      shutil.rmtree("../../source/0_thirdparty/rex_std")

  # # now run the install script.
  # os.system(f"py {os.path.join(new_wd, 'fetch_rexstd.py')} " + cwd)
  args = "-cwd="+cwd
  p = subprocess.Popen(["python.exe", os.path.join(new_wd, 'fetch_rexstd.py'), args], stdout=sys.stdout)
  p.communicate()

  # reset the working directory
  os.chdir(cwd)

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-update_rexpy", help="update rexpy, this is basically a reinstall", action="store_true")
  parser.add_argument("-update_rexstd", help="update rexstd, this is basically a reinstall", action="store_true")

  args, unknown = parser.parse_known_args()

  install_rexpy(args.update_rexpy)
  install_rexstd(args.update_rexstd)

  # now that rexpy and rexstd is installed, we can safely call the rest of the code

  arguments_to_pass_on = ""
  for arg in unknown:
    arguments_to_pass_on += f" {arg}"

  os.system(f"py build/scripts/rexpy/rexpy/setup.py{arguments_to_pass_on}")