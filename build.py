# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: build.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

import os
import argparse
import rexpy.build
import rexpy.util
import rexpy.diagnostics

from pathlib import Path

def __all_ninja_files(dir):
  ninja_files = rexpy.util.find_files_with_extension(dir, ".ninja")

  return ninja_files

def __is_special_test_file(file):
  if "_coverage" in file:
    return True

  if "_asan" in file:
    return True

  if "_ubsan" in file:
    return True

  if "_fuzzy" in file:
    return True

  return False

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  
  parser.add_argument("-clean", help="clean all intermediate files first", action="store_true")
  parser.add_argument("-unittests", help="generate unit tests", action="store_true")
  parser.add_argument("-coverage", help="generate coverage", action="store_true")
  parser.add_argument("-asan", help="generate address sanitizer", action="store_true")
  parser.add_argument("-ubsan", help="generate undefined behavior sanitizer", action="store_true")
  parser.add_argument("-fuzzy", help="generate fuzzy testing", action="store_true")

  args, unknown = parser.parse_known_args()

  ninja_files = __all_ninja_files(rexpy.util.find_root())

  build_any_special_tests = args.coverage or args.asan or args.ubsan or args.fuzzy

  if not args.coverage:
    ninja_files = [file for file in ninja_files if "_coverage" not in file]
  if not args.asan:
    ninja_files = [file for file in ninja_files if "_asan" not in file]
  if not args.ubsan:
    ninja_files = [file for file in ninja_files if "_ubsan" not in file]
  if not args.fuzzy:
    ninja_files = [file for file in ninja_files if "_fuzzy" not in file]
  if build_any_special_tests == True and not args.unittests:
    ninja_files = [file for file in ninja_files if __is_special_test_file(file)]

  result = 0

  for ninja_file in ninja_files:
    result |= rexpy.build.new_build(ninja_file, args.clean)

  if result != 0:
    rexpy.diagnostics.log_err("Not all builds succeeded")
  else:
    rexpy.diagnostics.log_info("Build successful")
