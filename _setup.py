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
import shutil
import subprocess
import sys
import time

def __get_script_path():
    return os.path.dirname(os.path.realpath(__file__))

def __rexpy_path():
  return os.path.join(__get_script_path(), os.path.join("build", os.path.join("scripts", "rexpy")))
def __rexstd_path():
  return os.path.join(__get_script_path(), os.path.join("source", os.path.join("0_thirdparty", "rex_std")))

def is_rexpy_installed():
  return os.path.exists(__rexpy_path())
def is_rexstd_installed():
  return os.path.exists(__rexstd_path())

def __fetch_rexpy():
  fetch_rexpy_path = os.path.join(__get_script_path(), os.path.join("build", os.path.join("scripts", "fetch_rexpy.py")))
  p = subprocess.Popen(["python.exe", fetch_rexpy_path], stdout=sys.stdout)
  p.communicate()
def __fetch_rexstd():
  fetch_rexstd_args = "-cwd=" + __get_script_path()
  fetch_rexstd_path = os.path.join(__get_script_path(), os.path.join("build", os.path.join("scripts", "fetch_rexstd.py")))
  p = subprocess.Popen(["python.exe", fetch_rexstd_path, fetch_rexstd_args], stdout=sys.stdout)
  p.communicate()

def install_rexpy(forceInstall):
  if forceInstall == False and is_rexpy_installed():
    print(f"rexpy is already installed - skipping install")
    return
  elif forceInstall == True:
    if is_rexpy_installed():
      print(f"remove the already install rexpy")
      shutil.rmtree(__rexpy_path())

  __fetch_rexpy()

def install_rexstd(forceInstall):
  if forceInstall == False and is_rexstd_installed():
    print(f"rexstd is already installed - skipping install")
    return
  elif forceInstall == True:
    if is_rexstd_installed():
      print(f"remove the already install rexstd")
      shutil.rmtree(__rexstd_path())

  __fetch_rexstd()

def __main():
  parser = argparse.ArgumentParser()
  parser.add_argument("-update_rexpy", help="update rexpy, this is basically a reinstall", action="store_true")
  parser.add_argument("-update_rexstd", help="update rexstd, this is basically a reinstall", action="store_true")

  args, unknown = parser.parse_known_args()

  # let's assume if "build" and "source"are found, we're in the root
  build_exists = os.path.exists("./build")
  source_exists = os.path.exists("./source")

  if not build_exists or not source_exists:
    print("Error: You're not running setup.py from the root directory. Please run this from the root directory and try again")
    return

  install_rexpy(args.update_rexpy)
  install_rexstd(args.update_rexstd)

  # # now that rexpy and rexstd is installed, we can safely call the rest of the code
  # arguments_to_pass_on = ""
  # for arg in unknown:
  #   arguments_to_pass_on += f" {arg}"

  # os.system(f"py build/scripts/rexpy/rexpy/setup.py{arguments_to_pass_on}")

if __name__ == "__main__":
  start_time = time.time()
  __main()
  print("Info: --- %s seconds (total) ---" % (time.time() - start_time))