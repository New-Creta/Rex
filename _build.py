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
import sys
import argparse
import regis.build
import regis.util
import regis.diagnostics
import regis.task_raii_printing

from pathlib import Path

def find_sln_in_cwd():
  dirs = os.listdir()
  for dir in dirs:
    if os.path.isfile(dir) and Path(dir).suffix == ".nsln":
      return dir
    
  return ""


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  
  parser.add_argument("-clean", help="clean all intermediate files first", action="store_true")
  parser.add_argument("-sln", default="", help="path to nsln file")
  parser.add_argument("-project", default="regina", help="project to build")
  parser.add_argument("-config", default="debug_opt", help="configuration to build for")
  parser.add_argument("-compiler", default="unknown", help="configuration to build with")

  args, unknown = parser.parse_known_args()

  sln_file = args.sln
  if sln_file == "":
    sln_file = find_sln_in_cwd()

  if args.compiler == "unknown":
    if regis.util.is_windows():
      args.compiler = "msvc"
    else:
      args.compiler = "clang"

  intermediate_dir = ""

  task = regis.task_raii_printing.TaskRaiiPrint("Building")
  result = regis.build.new_build(sln_file, args.project, args.config, args.compiler, intermediate_dir, args.clean)

  if result != 0:
    regis.diagnostics.log_err("Build failed")
    sys.exit(result)
  else:
    regis.diagnostics.log_info("Build successful")
  
