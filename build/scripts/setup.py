import os
import argparse
import rexpy.util
import rexpy.rex_json
import rexpy.required_tools
import rexpy.required_libs
import rexpy.install_externals
import shutil

root_path = rexpy.util.find_root()
settings = rexpy.rex_json.load_file(os.path.join(root_path, "build", "config", "settings.json"))
intermediate_dir = os.path.join(rexpy.util.find_root(), settings["intermediate_folder"])

def __clean_intermediate():
  # this clean the entire intermediate directory and all sub folders
  if os.path.exists(intermediate_dir):
    shutil.rmtree(intermediate_dir)

def run(shouldClean):
  if shouldClean:
    __clean_intermediate()

  # Time to install everything needed to build rex engine.
  # This is split into multiple steps

  # First we need to install the required build tools.
  # This includes make tools, build tools, compilers, linkers, ..
  rexpy.required_tools.run(False)

  # Next, install all required libraries for the compilers and platforms (eg. C++ standard library, Windows SDK)
  # Rex itself doesn't use C++ standard library, but thirdparty libraries might, so we still have to provide it.
  rexpy.required_libs.run()

  # Lastly we need to install all the externals used by rex itself
  # the difference between these libraries and the libraries of the previous step is that
  # we (Rex developers) decided that we need these libraries and we can decide to remove them at any point.
  # libraries like Windows SDK, C++ standard library is always required for development, where as libraries like ImGui
  # are not required and can always be replaced if a better library comes along.
  rexpy.install_externals.run()
      
if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-clean", help="clean setup, as if run for the first time", action="store_true")
  args, unknown = parser.parse_known_args()

  run(args.clean)