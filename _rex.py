# This is the general purpose python file
# This file is meant to be used by developers of rex engine.
# Running this file is the interface on how developers interact rex engine toolchain
# This file allows you to setup, generate, build, run and test the engine.

# It's possible rexpy is not installed yet when this file executes
# Other than testing if its installed, we can't use regis at all in this script.

# The rex workflow is the following:
#                              +--> Run
# Setup -> Generate -> Build --+ 
#                              +--> Test

import os
import argparse
import sys
import pkg_resources

# Check if we have regis installed.
# If not, we limit the amount of user input possibilities later on.
try:
  rexpy_installed = True
  import regis
except:
  rexpy_installed = False

required_rexpy_version = "0.1.46"

def _run_script(scriptPath : str, args : list[str]):
  abs_path = os.path.abspath(scriptPath)
  if not os.path.exists(abs_path):
    print(f'Error: script path "{abs_path}" doesn\'t exist.')
    print(f'This is possible if you\'re not running from the root of the repository')
    print(f'You\'re current working directory is: "{os.getcwd()}"')
    return
  
  return os.system(f"py {scriptPath} {' '.join(args)}")

def exec_version():
  root = os.path.dirname(__file__)
  rex_version_filename = os.path.join(root, 'rex.version')
  with open(rex_version_filename) as f:
    version = f.readline()
    print(f'Rex Engine version: {version}')

def _install_regis():
  os.system(f"py -m pip install --upgrade \"regis=={required_rexpy_version}\"")

def _correct_regis_installed():
  if not rexpy_installed:
    return False
  
  if not pkg_resources.get_distribution("regis").version == required_rexpy_version:
    return False
  
  return True

def exec_setup(argsToPassOn : list[str]):
  # As it's possible regis is not installed yet
  # We need to make sure we install it first.
  # After it's installed, we can call all other scripts
  # including the internal setup script.
  if not _correct_regis_installed():
    _install_regis()

  # Now that we have regis installed, 
  # We call the internal setup scripts
  internal_script_setup_path  = os.path.join('_build', 'scripts', 'setup.py')
  _run_script(internal_script_setup_path, argsToPassOn)

def exec_generate(argsToPassOn : str):
  internal_script_setup_path  = os.path.join('_build', 'scripts', 'generate.py')
  _run_script(internal_script_setup_path, argsToPassOn)
  return

def exec_build(argsToPassOn : str):
  internal_script_setup_path  = os.path.join('_build', 'scripts', 'build.py')
  _run_script(internal_script_setup_path, argsToPassOn)
  return

def exec_launch(argsToPassOn : str):
  internal_script_setup_path  = os.path.join('_build', 'scripts', 'run.py')
  _run_script(internal_script_setup_path, argsToPassOn)
  return

def exec_test(argsToPassOn : str):
  internal_script_setup_path  = os.path.join('_build', 'scripts', 'test.py')
  _run_script(internal_script_setup_path, argsToPassOn)
  return

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("-version", help="Display the version of the rex engine and exit", action="store_true")
  parser.add_argument("-setup", help="Perform the setup of the rex engine", action="store_true")
  parser.add_argument("-setup_arg", default=[], help="Arguments to pass on to setup script", action="append")

  if not rexpy_installed:
    print("Warning: rexpy not installed. Only setup is possible.")
  else:
    parser.add_argument("-generate", help="Generate the solution of rex engine", action="store_true")
    parser.add_argument("-generate_arg", default=[], help="Arguments to pass on to generate script", action="append")
    parser.add_argument("-build", help="Build the rex engine", action="store_true")
    parser.add_argument("-build_arg", default=[], help="Arguments to pass on to the build script", action="store_true")
    parser.add_argument("-launch", help="Launch a previous build project with the engine.", action="store_true")
    parser.add_argument("-launch_arg", default=[], help="Arguments to pass on to the launch script.", action="store_true")
    parser.add_argument("-test", help="Run a test on the engine.", action="store_true")
    parser.add_argument("-test_arg", default=[], help="Arguments to pass on to the test script.", action="store_true")

  args, unknown_args = parser.parse_known_args()

  if len(sys.argv) == 1:
    parser.print_help()
    exit(0)

  if args.version:
    exec_version()
    exit(0)
  
  if args.setup:
    exec_setup(args.setup_arg)

  if rexpy_installed:
    if args.generate:
      exec_generate(args.generate_arg)

    if args.build:
      exec_build(args.build_arg)

    if args.launch:
      exec_launch(args.launch_arg)

    if args.test:
      exec_test(args.test_arg)

  exit(0)

if __name__ == "__main__":
  main()