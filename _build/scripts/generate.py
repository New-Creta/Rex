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

def make_optional_arg(arg : str):
  return f'-{arg}'

def _add_config_arguments(parser : argparse.ArgumentParser, defaultConfig : dict):
  """Load the sharpmake config file from disk and add the options as arguments to this script."""
  
  for setting in defaultConfig:
    arg = make_optional_arg(setting)
    val = default_config[setting]['Value']
    desc = default_config[setting]['Description']
    if 'Options' in default_config[setting]:
      desc += f' Options: {default_config[setting]["Options"]}'
    
    if type(val) == bool:
      parser.add_argument(arg, help=desc, action='store_true')
    else:
      parser.add_argument(arg, help=desc, default=val)

def _create_new_config(defaultConfig : dict, args):
  """Create a config dictionary based on the arguments passed in."""

  config : dict = copy.deepcopy(defaultConfig)
  for arg in vars(args):
    arg_name = arg
    arg_name = arg_name.replace('_', '-') # python converts all hyphens into underscores so we need to convert them back
    arg_val = getattr(args, arg)

    if arg_name in config:
      config[arg_name]['Value'] = arg_val

  return config

def _save_config_file(settings : dict, config : dict):
  """Create a new config file. This file will be passed over to sharpmake"""

  config_dir = os.path.join(os.path.join(root, settings['intermediate_folder'], settings['build_folder']))
  if not os.path.exists(config_dir):
    os.mkdir(config_dir)

  config_path = os.path.join(config_dir, 'config.json')
  regis.rex_json.save_file(config_path, config)

  return config_path.replace('\\', '/')

if __name__ == "__main__":
  root = regis.util.find_root()
  default_config : dict = regis.rex_json.load_file(os.path.join(root, "_build", "sharpmake", "data", "default_config.json"))

  # initialize the argument parser by loading the arguments from the config file
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('-clean', help='Clean the intermediates before generation')
  _add_config_arguments(parser, default_config)

  # parse the arguments passed to the script
  args, unknown = parser.parse_known_args()
  config = _create_new_config(default_config, args)

  # save the config file to disk
  settings_path = os.path.join(root, "_build", "config", "settings.json")
  settings = regis.rex_json.load_file(settings_path)
  config_path = _save_config_file(settings, config)

  # call generation code to launch sharpmake
  sharpmake_args = f"/configFile(\"{config_path}\")"
  proc = regis.generation.new_generation(settings_path, sharpmake_args)
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