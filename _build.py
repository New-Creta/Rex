# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: build.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

import time
import argparse
import rexpy.build
import rexpy.util
import rexpy.diagnostics
import rexpy.task_raii_printing

from pathlib import Path

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  
  parser.add_argument("-clean", help="clean all intermediate files first", action="store_true")
  parser.add_argument("-project", help="project to build")
  parser.add_argument("-config", help="configuration to build for")
  parser.add_argument("-compiler", help="configuration to build with")

  args, unknown = parser.parse_known_args()

  task = rexpy.task_raii_printing.TaskRaiiPrint("Building")
  result = rexpy.build.new_build(args.project, args.config, args.compiler, args.clean)

  if result != 0:
    rexpy.diagnostics.log_err("Build failed")
  else:
    rexpy.diagnostics.log_info("Build successful")
  
