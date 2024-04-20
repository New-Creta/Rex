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
import subprocess
import importlib.metadata

# Check if we have regis installed.
# If not, we limit the amount of user input possibilities later on.
try:
  rexpy_installed = True
  import regis
except:
  rexpy_installed = False

required_rexpy_version = "0.1.85"

# all scripts are located in ~/_build/scripts path.
# to make it easier to call these scripts wherever we need them
# we cache their paths here.
root = os.path.dirname(__file__)
scripts_path = os.path.join(root, '_build', 'scripts')
setup_script_path = os.path.join(scripts_path, 'setup.py')
validate_script_path = os.path.join(scripts_path, 'validate.py')
generate_script_path = os.path.join(scripts_path, 'generate.py')
build_script_path = os.path.join(scripts_path, 'build.py')
launch_script_path = os.path.join(scripts_path, 'launch.py')
unit_test_script_path = os.path.join(scripts_path, 'unit_test.py')
auto_test_script_path = os.path.join(scripts_path, 'auto_test.py')
fuzzy_test_script_path = os.path.join(scripts_path, 'fuzzy_test.py')
code_analysis_script_path = os.path.join(scripts_path, 'code_analysis.py')

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
  
  script_args = []
  script_args.append('python')
  script_args.append(scriptPath)
  script_args.extend(args)
  proc = subprocess.Popen(args=script_args)
  proc.wait()

  return proc.returncode

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
  
  if not importlib.metadata.distribution("regis").version == required_rexpy_version:
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
  return _run_script(setup_script_path, argsToPassOn)

def _exec_validate(argsToPassOn : str):
  """Execute the internal validation script"""
  return _run_script(validate_script_path, argsToPassOn)

def _exec_generate(argsToPassOn : str):
  """Execute the internal generate script"""
  return _run_script(generate_script_path, argsToPassOn)

def _exec_build(argsToPassOn : str):
  """Execute the internal build script"""
  return _run_script(build_script_path, argsToPassOn)

def _exec_launch(argsToPassOn : str):
  """Execute the internal launch script"""
  return _run_script(launch_script_path, argsToPassOn)

def _exec_unit_tests(argsToPassOn : str):
  """Execute the internal unit test script"""
  return _run_script(unit_test_script_path, argsToPassOn)

def _exec_auto_tests(argsToPassOn : str):
  """Execute the internal auto test script"""
  return _run_script(auto_test_script_path, argsToPassOn)

def _exec_fuzzy_tests(argsToPassOn : str):
  """Execute the internal fuzzy test script"""
  return _run_script(fuzzy_test_script_path, argsToPassOn)

def _exec_code_analysis(argsToPassOn : str):
  """Execute the internal code analysis script"""
  return _run_script(code_analysis_script_path, argsToPassOn)

def main():
  # look into sub parsers
  parser = argparse.ArgumentParser()
  parser.add_argument("-version", help="Display the version of the rex engine and exit", action="store_true")
  command_subparser = parser.add_subparsers(dest='command')

  command_subparser.add_parser('setup', help='Perform the setup of the rex engine', add_help=False)

  if not rexpy_installed:
    print("Warning: rexpy not installed. Only setup is possible.")
  else:
    command_subparser.add_parser("validate", help="Validate the setup of rex.", add_help=False)
    command_subparser.add_parser("generate", help="Generate the solution of rex engine", add_help=False)
    command_subparser.add_parser("build", help="Build the rex engine", add_help=False)
    command_subparser.add_parser("launch", help="Launch a previous build project with the engine.", add_help=False)
    command_subparser.add_parser("unit-test", help="Run unit tests on the engine and projects.", add_help=False)
    command_subparser.add_parser("auto-test", help="Run auto tests on the engine and projects.", add_help=False)
    command_subparser.add_parser("fuzzy-test", help="Run fuzzy tests on the engine and projects.", add_help=False)
    command_subparser.add_parser("code-analysis", help="Run code analysis on the engine and projects.", add_help=False)

  args, unknown_args = parser.parse_known_args()

  # If only 1 argument is provided (the path to the module)
  # Print the arguments of this script (in essence, display how it should be used)
  # And then exit.
  # This is similar as passing in -h or --help to this script
  if len(sys.argv) == 1:
    parser.print_help()
    exit(0)

  if args.version:
    _exec_version()
    exit(0)
  
  res = 0
  if args.command == 'setup':
    res = _exec_setup(unknown_args)

  if rexpy_installed:
    if args.command == 'validate':
      res = _exec_validate(unknown_args)

    if args.command == 'generate':
      res = _exec_generate(unknown_args)

    if args.command == 'build':
      res = _exec_build(unknown_args)

    if args.command == 'launch':
      res = _exec_launch(unknown_args)

    if args.command == 'unit-test':
      res = _exec_unit_tests(unknown_args)

    if args.command == 'auto-test':
      res = _exec_auto_tests(unknown_args)

    if args.command == 'fuzzy-test':
      res = _exec_fuzzy_tests(unknown_args)

    if args.command == 'code-analysis':
      res = _exec_code_analysis(unknown_args)

  sys.exit(res)

if __name__ == "__main__":
  main()