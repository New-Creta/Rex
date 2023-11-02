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
import copy
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
  regis.generation.add_config_arguments_to_parser(parser)

  # parse the arguments passed to the script
  args, unknown = parser.parse_known_args()
  config = regis.generation.create_config(args)

  # call generation code to launch sharpmake
  settings_path = os.path.join(root, "_build", "config", "settings.json")
  proc = regis.generation.new_generation(settings_path, config)
  proc.wait()
  result = proc.returncode

  # This setup is optional, but for users using VSCode as an IDE we copy over a "tasks.json" file
  # this file will help in running the different scripts to generate/build/test rex
  # Should maybe be moved to the generation step and not the setup step
  vscode_dir = os.path.join(root, ".vscode")
  vscode_build_dir = os.path.join(root, "_build", "vscode")
  if not os.path.exists(vscode_dir):
    os.mkdir(vscode_dir)
  shutil.copyfile(os.path.join(vscode_build_dir, "tasks.json"), os.path.join(vscode_dir, "tasks.json"))  


  exit(result)
