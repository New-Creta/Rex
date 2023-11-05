# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: build.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

# This script acts as the interface to build the rex engine and its tools
# it supports setting the compiler, configuration as well as the project to build.

import os
import sys
import argparse
import regis.build
import regis.util
import regis.diagnostics
import regis.task_raii_printing
import regis.dir_watcher
import regis.rex_json

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  
  # There a couple ways of calling this script, but in essence it's very simple
  # build.py                 <-- builds the project
  # build.py -clean          <-- rebuilds the project
  # build.py -nobuild -clean <-- cleans the project
  parser.add_argument("-nobuild", help="Don't perform a build of the project", action="store_true")
  parser.add_argument("-clean", help="clean all intermediate files", action="store_true")
  parser.add_argument("-sln", default="", help="path to nsln file")
  parser.add_argument("-project", default="regina", help="project to build")
  parser.add_argument("-config", default="debug_opt", help="configuration to build for")
  parser.add_argument("-compiler", default="unknown", help="configuration to build with")
  parser.add_argument("-dont_build_dependencies", default=False, help="build only the project specified and not its dependencies", action="store_true")

  args, unknown = parser.parse_known_args()

  if args.compiler == "unknown":
    if regis.util.is_windows():
      args.compiler = "msvc"
    else:
      args.compiler = "clang"

  task = regis.task_raii_printing.TaskRaiiPrint("Building")
  result = regis.build.new_build(args.project, args.config, args.compiler, not args.nobuild, args.clean, args.sln, not args.dont_build_dependencies)

  if result != 0:
    regis.diagnostics.log_err("Build failed")
  else:
    regis.diagnostics.log_info("Build successful")
  
  sys.exit(result)
