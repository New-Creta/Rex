# This is the general purpose python file
# This file is meant to be used by developers of rex engine.
# Running this file is the interface on how developers interact rex engine toolchain
# This file allows you to setup, generate, build, run and test the engine.

# It's possible rexpy is not installed yet when this file executes

import os
import argparse

def exec_setup():
  print(f"the user choose setup")
  return

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

def ask_user_action():
  print('What would you like to do?')
  print('1) Setup')
  print('2) Generate')
  print('3) Build')
  print('4) Run')
  print('5) Test')
  return input()

def launch_user_action(userAction : str):
  userAction = userAction.lower()
  if "setup" == userAction:
    exec_setup()
  elif "generate" == userAction:
    exec_generate()
  elif "build" == userAction:
    exec_build()
  elif "run" in userAction:
    exec_run()
  elif "test" in userAction:
    exec_test()
  else:
    print("I'm sorry, I didn't get that.")
    return False
  
  return True

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("-clean", help="clean setup, as if run for the first time", action="store_true")
  args, unknown = parser.parse_known_args()

  has_user_chosen_an_action = False
  while not has_user_chosen_an_action:
    chosen_action = ask_user_action()
    has_user_chosen_an_action = launch_user_action(chosen_action)

if __name__ == "__main__":
  main()