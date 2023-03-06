import os
import argparse
import regis.util
import regis.rex_json
import regis.required_tools
import regis.required_libs
import regis.install_externals
import regis.diagnostics
import regis.task_raii_printing
import regis.git_hooks
import shutil

root_path = regis.util.find_root()
settings = regis.rex_json.load_file(os.path.join(root_path, "build", "config", "settings.json"))
intermediate_dir = os.path.join(regis.util.find_root(), settings["intermediate_folder"])
misc_folders = settings["misc_folders"]
misc_extensions = settings["misc_extensions"]

def __clean_intermediate():
  regis.diagnostics.log_info(f"cleaning intermediates")
  
  # this clean the entire intermediate directory and all sub folders
  if os.path.exists(intermediate_dir):
    shutil.rmtree(intermediate_dir)

  for misc_folder in misc_folders:
    if os.path.exists(misc_folder):
      shutil.rmtree(misc_folder)

  files = os.listdir()
  for file in files:
    if os.path.isfile(file):
      for misc_extension in misc_extensions:
        if misc_extension in file:
          os.remove(file)

def run(shouldClean):
  if shouldClean:
    task = regis.task_raii_printing("cleaning.")
    __clean_intermediate()

  task = regis.task_raii_printing.TaskRaiiPrint("running setup")

  # Time to install everything needed to build rex engine.
  # This is split into multiple steps

  # First we need to install the required build tools.
  # This includes make tools, build tools, compilers, linkers, ..
  regis.required_tools.run()

  # Next, install all required libraries for the compilers and platforms (eg. C++ standard library, Windows SDK)
  # Rex itself doesn't use C++ standard library, but thirdparty libraries might, so we still have to provide it.
  regis.required_libs.run()

  # Next we need to install all the externals used by rex itself
  # the difference between these libraries and the libraries of the previous step is that
  # we (Rex developers) decided that we need these libraries and we can decide to remove them at any point.
  # libraries like Windows SDK, C++ standard library is always required for development, where as libraries like ImGui
  # are not required and can always be replaced if a better library comes along.
  regis.install_externals.run()
      
  # Lastly, install the git hooks
  regis.git_hooks.run(os.path.join(root_path, "build", "scripts", "git", "hooks"))
  
if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-clean", help="clean setup, as if run for the first time", action="store_true")
  args, unknown = parser.parse_known_args()

  run(args.clean)