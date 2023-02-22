# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: generate.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

import os
import argparse
import rexpy.generation
import rexpy.rex_json
import rexpy.util
import glob
from pathlib import Path

def __find_sharpmake_files(directory):
  sharpmakes_files = []
  for root, dirs, files in os.walk(directory):
    for file in files:
      extensions = Path(file).suffixes
      if len(extensions) == 2:
        if extensions[0] == ".sharpmake" and extensions[1] == ".cs":
          files = glob.glob(os.path.join(directory, "**", file), recursive=True)
          
          sharpmakes_files.append(files[0])
  
  return sharpmakes_files

def __find_sharpmake_root_files(directory):
  sharpmakes_files = []
  for root, dirs, files in os.walk(directory):
    for file in files:
      extensions = Path(file).suffixes
      if len(extensions) == 1:
        if extensions[0] == ".cs":
          files = glob.glob(os.path.join(directory, "**", file), recursive=True)
          
          sharpmakes_files.append(files[0])

  return sharpmakes_files


if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument("-unittests", help="generate unit tests", action="store_true")
  parser.add_argument("-coverage", help="generate coverage", action="store_true")
  parser.add_argument("-asan", help="generate address sanitizer", action="store_true")
  parser.add_argument("-ubsan", help="generate undefined behavior sanitizer", action="store_true")
  parser.add_argument("-fuzzy", help="generate fuzzy testing", action="store_true")

  args, unknown = parser.parse_known_args()

  root = rexpy.util.find_root()
  settings = rexpy.rex_json.load_file(os.path.join(root, "build", "config", "settings.json"))
  sharpmake_root = os.path.join(root, settings["build_folder"], "sharpmake")
  source_root = os.path.join(root, settings["source_folder"])
  tests_root = os.path.join(root, settings["tests_folder"])
  sharpmakes_files = []
  sharpmakes_files.extend(__find_sharpmake_root_files(sharpmake_root))
  sharpmakes_files.extend(__find_sharpmake_files(source_root))
  sharpmakes_files.extend(__find_sharpmake_files(tests_root))

  run_any_tests = args.unittests or args.coverage or args.asan or args.ubsan or args.fuzzy

  if run_any_tests == False:
    rexpy.generation.new_generation(sharpmakes_files, "")

  if args.unittests:
    rexpy.generation.new_generation(sharpmakes_files, "/generateUnitTests")
  
  if args.coverage:
    rexpy.generation.new_generation(sharpmakes_files, "/generateUnitTests /enableCoverage")

  if args.asan:
    rexpy.generation.new_generation(sharpmakes_files, "/generateUnitTests /enableAddressSanitizer")

  if args.ubsan:
    rexpy.generation.new_generation(sharpmakes_files, "/generateUnitTests /enableUBSanitizer")

  if args.fuzzy:
    rexpy.generation.new_generation(sharpmakes_files, "/enableFuzzyTesting")
