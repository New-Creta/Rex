import sys
import subprocess
import pkg_resources

def run():
  print("Installing python modules")

  required = {'requests'}
  installed = {pkg.key for pkg in pkg_resources.working_set}
  missing = required - installed

  if missing:
    for missingModule in missing:
      print(f"installing \"{missingModule}\"")

    python = sys.executable
    subprocess.check_call([python, '-m', 'pip', 'install', *missing], stdout=subprocess.DEVNULL)

  print("Installing python modules - Done")
