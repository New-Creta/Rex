# This is the general purpose python file
# This file is meant to be used by developers of rex engine.
# Running this file is the interface on how developers interact rex engine toolchain
# This file allows you to setup, generate, build, run and test the engine.

# It's possible rexpy is not installed yet when this file executes

import os
import argparse

try:
  rexpy_installed = True
  import regis
except:
  rexpy_installed = False

def __install_regis():
  rexpy_version = "0.1.45"
  os.system(f"py -m pip install --upgrade \"regis=={rexpy_version}\"")

def exec_setup():
  __install_regis()
  global rexpy_installed
  rexpy_installed = True

def exec_generate():
  print(f"the user choose generate")
  return

def exec_build():
  print(f"the user choose build")
  return

def exec_run():
  print(f"the user choose run")
  return

def exec_test():
  print(f"the user choose test")
  return

def exec_exit():
  exit(0)

def ask_user_action():
  print('What would you like to do?')
  print('- Setup')

  if rexpy_installed:
    print('- Generate')
    print('- Build')
    print('- Run')
    print('- Test')
  
  print('- Exit')
  
  return input()

def launch_user_action(userAction : str):
  userAction = userAction.lower()
  if "setup" == userAction:
    exec_setup()

  if rexpy_installed:
    if "generate" == userAction:
      exec_generate()
    elif "build" == userAction:
      exec_build()
    elif "run" in userAction:
      exec_run()
    elif "test" in userAction:
      exec_test()

  if "exit" == userAction:
    exec_exit()
    return True

  print("I'm sorry, I didn't get that.")
  return False

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("-clean", help="clean setup, as if run for the first time", action="store_true")
  args, unknown = parser.parse_known_args()

  if not rexpy_installed:
    print("rexpy not installed. Only setup is possible.")

  should_exit = False
  while not should_exit:
    chosen_action = ask_user_action()
    should_exit = launch_user_action(chosen_action)

if __name__ == "__main__":
  main()