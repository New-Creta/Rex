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
import sys

if __name__ == "__main__":
  root = regis.util.find_root()

  # initialize the argument parser by loading the arguments from the config file
  parser = argparse.ArgumentParser(add_help=False, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('-sharpmake_arg', dest="sharpmake_args", default=[], action="append", help='Argument to be passed to sharpmake directly')
  parser.add_argument('-use-default-config', default=False, action="store_true", help='Use the default config as a initial settings')
  parser.add_argument('-h', '--help', action='store_true', dest='show_help', help='Show this help message and exit')

  # Do a first pass on the argument so we know if we should load the default config or not
  args, unknown = parser.parse_known_args()

  # Load the arguments from either the default config or the previous config
  regis.generation.add_config_arguments_to_parser(parser, args.use_default_config)

  # parse the arguments passed to this script
  args, unknown = parser.parse_known_args()

  # Now you can check if the help was requested
  if args.show_help:
    parser.print_help()
    exit()

  # Create the config dict
  config = regis.generation.create_config(args, args.use_default_config)

  # call generation code to launch sharpmake
  settings_path = os.path.join(root, regis.util.settingsPathFromRoot)
  settings = regis.rex_json.load_file(settings_path)

  # Sharpmake expects to be run from the root directory
  with regis.util.temp_cwd(root):
    result = regis.generation.new_generation(settings, config, args.sharpmake_args)

  exit(result)
