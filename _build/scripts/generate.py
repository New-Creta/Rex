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
import copy
import regis.generation
import regis.rex_json
import regis.util
import regis.diagnostics
import glob
from pathlib import Path

def make_optional_arg(arg : str):
  return f'-{arg}'

def _add_config_arguments(parser : argparse.ArgumentParser, defaultConfig : dict):
  for setting in defaultConfig:
    arg = make_optional_arg(setting)
    val = default_config[setting]['value']
    desc = default_config[setting]['description']
    if 'options' in default_config[setting]:
      desc += f' Options: {default_config[setting]["options"]}'
    
    if type(val) == bool:
      parser.add_argument(arg, help=desc, action='store_true')
    else:
      parser.add_argument(arg, help=desc, default=val)

def _create_new_config(defaultConfig : dict, args):
  config : dict = copy.deepcopy(default_config)
  for arg in vars(args):
    arg_name = arg
    arg_name = arg_name.replace('_', '-') # python converts all hyphens into underscores so we need to convert them back
    arg_val = getattr(args, arg)

    if arg_name in config:
      config[arg_name]['value'] = arg_val

  return config

def _save_config_file(settings : dict, config : dict):
  config_dir = os.path.join(os.path.join(root, settings['intermediate_folder'], settings['build_folder']))
  if not os.path.exists(config_dir):
    os.mkdir(config_dir)

  config_path = os.path.join(config_dir, 'config.json')
  regis.rex_json.save_file(config_path, config)

  return config_path

if __name__ == "__main__":
  root = regis.util.find_root()
  default_config : dict = regis.rex_json.load_file(os.path.join(root, "_build", "sharpmake", "data", "default_config.json"))

  # initialize the argument parser by loading the arguments from the config file
  parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
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
  sharpmake_args = f"/configFile({config_path})"
  proc = regis.generation.new_generation(settings_path, sharpmake_args)
  proc.wait()
  result = proc.returncode

  exit(result)
