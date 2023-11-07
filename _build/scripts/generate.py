# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: generate.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

# This script acts as the interface into sharpmake generation code.
# usually called by the _rex.py script that sits on the root.
# in short, it loads the template sharpmake configure script
# sets its values based on the command line passed in to this script
# and then calls sharpmake with that config file.

import os
import argparse
import regis.generation
import regis.rex_json
import regis.util
import regis.diagnostics
import shutil

if __name__ == "__main__":
  root = regis.util.find_root()

  # initialize the argument parser by loading the arguments from the config file
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('-clean', help='Clean the intermediates before generation')
  parser.add_argument('-sharpmake_arg', dest="sharpmake_args", default=[], action="append", help='Argument to be passed to sharpmake directly')
  regis.generation.add_config_arguments_to_parser(parser)

  # parse the arguments passed to this script
  args, unknown = parser.parse_known_args()
  config = regis.generation.create_config(args)

  # call generation code to launch sharpmake
  settings_path = os.path.join(root, regis.util.settingsPathFromRoot)
  proc = regis.generation.new_generation(settings_path, config, args.sharpmake_args)
  proc.wait()
  result = proc.returncode

  exit(result)
