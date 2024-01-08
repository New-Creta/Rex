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
  parser.add_argument('-sharpmake_arg', dest="sharpmake_args", default=[], action="append", help='Argument to be passed to sharpmake directly')
  parser.add_argument('-no_default_config', default=False, action="store_true", help='Don\'t start from the default config, but from what\'t previously configured')
  regis.generation.add_config_arguments_to_parser(parser)

  # parse the arguments passed to this script
  args, unknown = parser.parse_known_args()

  # if we're not allowed to use the default config, we need to mask of to only the args passed in
  disallow_default_config = args.no_default_config
  sharpmake_args = args.sharpmake_args
  if disallow_default_config:
    args_vars = vars(args)
    nothing = object()
    mask = argparse.Namespace(**{arg: nothing for arg in args_vars})
    masked_namespace = parser.parse_args(namespace=mask)
    masked_args = {
        arg: value
        for arg, value in vars(masked_namespace).items()
        if value is not nothing
    }
    args = argparse.Namespace(**masked_args)

  config = regis.generation.create_config(args, useDefault=not disallow_default_config)

  # call generation code to launch sharpmake
  settings_path = os.path.join(root, regis.util.settingsPathFromRoot)
  settings = regis.rex_json.load_file(settings_path)

  # Sharpmake expects to be run from the root directory
  with regis.util.temp_cwd(root):
    result = regis.generation.new_generation(settings, config, sharpmake_args)

  exit(result)
