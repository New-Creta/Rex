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

required_rexpy_version = "0.1.48"

# all scripts are located in ~/_build/scripts path.
# to make it easier to call these scripts wherever we need them
# we cache their paths here.
root = os.path.dirname(__file__)
scripts_path = os.path.join(root, '_build', 'scripts')
setup_script_path = os.path.join(scripts_path, 'setup.py')
generate_script_path = os.path.join(scripts_path, 'generate.py')
build_script_path = os.path.join(scripts_path, 'build.py')
launch_script_path = os.path.join(scripts_path, 'launch.py')
test_script_path = os.path.join(scripts_path, 'test.py')

def _run_script(scriptPath : str, args : list[str]):
  """Run the script if it exists. Warn if it doesn't."""
  abs_path = os.path.abspath(scriptPath)

  # to avoid human error during development, we check if the path actually exists.
  # If not, this usually means the user is not running from the root directory of rex.
  if not os.path.exists(abs_path):
    print(f'Error: script path "{abs_path}" doesn\'t exist.')
    print(f'This is possible if you\'re not running from the root of the repository')
    print(f'You\'re current working directory is: "{os.getcwd()}"')
    return
  
  return os.system(f"py {scriptPath} {' '.join(args)}")

def _exec_ext_help(parser : argparse.ArgumentParser):
  """Display the arguments of this script and all the internal scripts"""
  # First print the argument of this script.
  parser.print_help()

  # next print the arguments of each script we support
  print(f'-------------------------')
  print(f'Extended help info for scripts')
  print(f'-------------------------')
  print(f'SETUP')
  print(f'-------------------------')
  _run_script(setup_script_path, ['-h'])
  print(f'-------------------------')
  print(f'GENERATE')
  print(f'-------------------------')
  _run_script(generate_script_path, ['-h'])
  print(f'-------------------------')
  print(f'BUILD')
  print(f'-------------------------')
  _run_script(build_script_path, ['-h'])
  print(f'-------------------------')
  print(f'LAUNCH')
  print(f'-------------------------')
  _run_script(launch_script_path, ['-h'])
  print(f'-------------------------')
  print(f'TEST')
  print(f'-------------------------')
  _run_script(test_script_path, ['-h'])
  print(f'-------------------------')

def _exec_version():
  """Load the engine version and display it"""
  # The version of the engine is stored in rex.version file
  # which is located at the root of the engine.
  # This is a convenient way to query the engine version
  # from various different points without having to hardcode
  # it every time.
  root = os.path.dirname(__file__)
  rex_version_filename = os.path.join(root, 'rex.version')
  with open(rex_version_filename) as f:
    version = f.readline()
    print(f'Rex Engine version: {version}')

def _install_regis():
  """Install the required version of regis. No checking gets performed"""
  os.system(f"py -m pip install --upgrade \"regis=={required_rexpy_version}\"")

def _correct_regis_installed():
  """Verify if regis is installed and if it's installed, that it's the right version."""
  if not rexpy_installed:
    return False
  
  if not pkg_resources.get_distribution("regis").version == required_rexpy_version:
    return False
  
  return True

def _exec_setup(argsToPassOn : list[str]):
  """Install regis if needed. Execute the internal setup script afterwards."""
  # As it's possible regis is not installed yet
  # We need to make sure we install it first.
  # After it's installed, we can call all other scripts
  # including the internal setup script.
  if not _correct_regis_installed():
    _install_regis()

  # Now that we have regis installed, 
  # We call the internal setup scripts
  _run_script(setup_script_path, argsToPassOn)

def _exec_generate(argsToPassOn : str):
  """Execute the internal generate script"""
  _run_script(generate_script_path, argsToPassOn)
  return

def _exec_build(argsToPassOn : str):
  """Execute the internal build script"""
  _run_script(build_script_path, argsToPassOn)
  return

def _exec_launch(argsToPassOn : str):
  """Execute the internal launch script"""
  _run_script(launch_script_path, argsToPassOn)
  return

def _exec_test(argsToPassOn : str):
  """Execute the internal test script"""
  _run_script(test_script_path, argsToPassOn)
  return

def main():
  # look into sub parsers
  parser = argparse.ArgumentParser()
  parser.add_argument("-ext_help", help="Display extended help information and exit", action="store_true")
  parser.add_argument("-version", help="Display the version of the rex engine and exit", action="store_true")
  parser.add_argument("-setup", help="Perform the setup of the rex engine", action="store_true")
  parser.add_argument("-setup_arg", default=[], help="Arguments to pass on to setup script", action="append")

  if not rexpy_installed:
    print("Warning: rexpy not installed. Only setup is possible.")
  else:
    parser.add_argument("-generate", help="Generate the solution of rex engine", action="store_true")
    parser.add_argument("-generate_arg", default=[], help="Arguments to pass on to generate script", action="append")
    parser.add_argument("-build", help="Build the rex engine", action="store_true")
    parser.add_argument("-build_arg", default=[], help="Arguments to pass on to the build script", action="append")
    parser.add_argument("-launch", help="Launch a previous build project with the engine.", action="store_true")
    parser.add_argument("-launch_arg", default=[], help="Arguments to pass on to the launch script.", action="append")
    parser.add_argument("-test", help="Run a test on the engine.", action="store_true")
    parser.add_argument("-test_arg", default=[], help="Arguments to pass on to the test script.", action="append")

  args, unknown_args = parser.parse_known_args()

  # If only 1 argument is provided (the path to the module)
  # Print the arguments of this script (in essence, display how it should be used)
  # And then exit.
  # This is similar as passing in -h or --help to this script
  if len(sys.argv) == 1:
    parser.print_help()
    exit(0)

  if args.ext_help:
    _exec_ext_help(parser)
    exit(0)

  if args.version:
    _exec_version()
    exit(0)
  
  if args.setup:
    _exec_setup(args.setup_arg)

  if rexpy_installed:
    if args.generate:
      _exec_generate(args.generate_arg)

    if args.build:
      _exec_build(args.build_arg)

    if args.launch:
      _exec_launch(args.launch_arg)

    if args.test:
      _exec_test(args.test_arg)

  exit(0)

if __name__ == "__main__":
  main()