# This is the internal setup script used by Rex Engine.
# This script is meant to be called by _rex.py,
# which sits in the root directory of the engine
# and is not supposed be called directly by the user.
#
# This script's is responsible for preparing current working directory
# for Rex Engine development.
# This includes but is not limited to:
# - Installing tools (eg. Compilers, Linkers, ..)
# - Installing libraries (eg. Windows SDK, ..)
# - Installing externals (eg. rexstd, glm, ..)
# - Initializing supported IDEs (Eg. VS Code)
#
# It's possible the functionality of this script might change in the future
# Time of writing - [06/Oct/2023] - Nick DB

import os
import argparse
import sys
import regis.util
import regis.rex_json
import regis.required_tools
import regis.required_libs
import regis.required_externals
import regis.diagnostics
import regis.task_raii_printing
import regis.git_hooks
import shutil

root_path = regis.util.find_root()
settings = regis.rex_json.load_file(os.path.join(root_path, regis.util.settingsPathFromRoot))
intermediate_dir = os.path.join(root_path, settings["intermediate_folder"])

misc_folders = settings["misc_folders"]
misc_extensions = settings["misc_extensions"]

def _exec_clean():
  """Remove all the files in the intermediate directory"""
  with regis.task_raii_printing.TaskRaiiPrint(f"cleaning intermediates"):
    try:
      with regis.util.LoadingAnimation('Cleaning'):        
        # this clean the entire intermediate directory and all sub folders
        if os.path.exists(intermediate_dir):
          regis.diagnostics.log_info(f'Cleaning {intermediate_dir}')
          shutil.rmtree(intermediate_dir)

        # There are a few misc folders other than our own intermediate directory
        # that need to get cleaned (eg. .vscode folder)
        for misc_folder in misc_folders:
          if os.path.exists(misc_folder):
            regis.diagnostics.log_info(f'Cleaning {misc_folder}')
            shutil.rmtree(misc_folder)

        # There are also a few files generated in the root
        # that need to get removed when performing a clean setup
        # eg. the visual studio solution of the engine.
        files = os.listdir()
        for file in files:
          if os.path.isfile(file):
            for misc_extension in misc_extensions:
              if misc_extension in file:
                regis.diagnostics.log_info(f'Cleaning {file}')
                os.remove(file)
    except Exception as ex:
      regis.diagnostics.log_err(f'Failed to clean intermediates: {ex}')
      exit(1)
  
def _exec_query():
  """Query what still needs to get installed."""

  regis.diagnostics.log_no_color('--------------------------------')
  regis.required_tools.query()

  regis.diagnostics.log_no_color('--------------------------------')
  regis.required_libs.query()
  
  regis.diagnostics.log_no_color('--------------------------------')
  regis.required_externals.query()
  
  regis.diagnostics.log_no_color('--------------------------------')

def _exec_run():
  """Run the actual setup"""

  with regis.task_raii_printing.TaskRaiiPrint("running setup"):

    # Time to install everything needed to build rex engine.
    # This is split into multiple steps

    # First we need to install the required build tools.
    # This includes make tools, build tools, compilers, linkers, ..
    regis.required_tools.install()

    # Next, install all required libraries for the compilers and platforms (eg. C++ standard library, Windows SDK)
    # Rex itself doesn't use C++ standard library, but thirdparty libraries might, so we still have to provide it.
    regis.required_libs.install()

    # Next we need to install all the externals used by rex itself
    # the difference between these libraries and the libraries of the previous step is that
    # we (Rex developers) decided that we need these libraries and we can decide to remove them at any point.
    # libraries like Windows SDK, C++ standard library is always required for development, where as libraries like ImGui
    # are not required and can always be replaced if a better library comes along.
    regis.required_externals.install()
        
    # Lastly, install the git hooks
    regis.git_hooks.run(os.path.join(root_path, "_build", "scripts", "git", "hooks"))

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument("-query", help="Don't display any options and just run the script", action="store_true")
  parser.add_argument("-clean", help="clean setup, as if run for the first time", action="store_true")
  args, unknown = parser.parse_known_args()

  if args.query:
    _exec_query()
    exit(0)

  if args.clean:
    _exec_clean()

  _exec_run()  

  exit(0)

