# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: run_clang_tools.py
# Copyright (c) Nick De Breuck 2022
#
# ============================================

import os
import argparse
import subprocess
import diagnostics
import util
import logging

logger = diagnostics.StreamLogger("logger", logging.INFO)

def __run_command(command):
  proc = subprocess.Popen(command)
  streamdata = proc.communicate()[0]
  return proc.returncode


def run(projectName, config):
  script_path = os.path.dirname(__file__)
  root_path = util.find_in_parent(script_path, "source")

  logger.info("Running clang-tidy - first pass")
  rc = __run_command(f"py {script_path}/run_clang_tidy.py -config-file={root_path}/source/.clang-tidy_first_pass -p={root_path}/.rex/build/ninja/{projectName}/clang_tools/clang/{config} -header-filter=.* -quiet -fix") # force clang compiler, as clang-tools expect it

  if rc != 0:
    raise Exception("clang-tidy first pass failed")
  
  logger.info("Running clang-tidy - second pass")
  rc = __run_command(f"py {script_path}/run_clang_tidy.py -config-file={root_path}/source/.clang-tidy_second_pass -p={root_path}/.rex/build/ninja/{projectName}/clang_tools/clang/{config} -header-filter=.* -quiet") # force clang compiler, as clang-tools expect it

  if rc != 0:
    raise Exception("clang-tidy second pass failed")

  logger.info("Running clang-format")
  rc = __run_command(f"py {script_path}/run_clang_format.py -r -i {root_path}/source/{projectName}")

  if rc != 0:
    raise Exception("clang-format failed")

if __name__ == "__main__":
  # arguments setups
  parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)

  parser.add_argument("-p", "--project", help="project name")
  parser.add_argument("-conf", "--config", help="configuration")
  args, unknown = parser.parse_known_args()

 # useful for debugging
  logger.info(f"Executing {__file__}")

 # execute the script
  run(args.project, args.config)

 # print. We're done.
  logger.info("Done.")

  exit(0)