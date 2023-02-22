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
  parser.add_argument("-project", help="project to build")
  parser.add_argument("-config", help="configuration to build for")
  parser.add_argument("-compiler", help="configuration to build with")

  args, unknown = parser.parse_known_args()
  result = rexpy.build.new_build(args.project, args.config, args.compiler, args.clean)

  if result != 0:
    rexpy.diagnostics.log_err("Build failed")
  else:
    rexpy.diagnostics.log_info("Build successful")
