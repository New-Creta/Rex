# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: build.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

# This script acts as the interface to build the rex engine and its tools
# it supports launching previous build projects

import sys
import argparse
import os
import pathlib
import subprocess
import regis.util
import regis.rex_json
import regis.diagnostics

# Build projects and their paths are specified in a json file under the build path
# This gets generated during the build phase.
# This script uses that file to determine which projects exist and can be launched
root = regis.util.find_root()  
settings_path = os.path.join(root, regis.util.settingsPathFromRoot)
settings = regis.rex_json.load_file(settings_path)
intermediate_path = os.path.join(root, settings['intermediate_folder'], settings['build_folder'])
build_projects_path = os.path.join(intermediate_path, settings['build_projects_filename'])

def display_program_options(programPaths : list[str]):
  """Display all the possible path options to the user"""
  regis.diagnostics.log_warn(f'Options:')
  for path in programPaths:
    program = pathlib.Path(path).name
    regis.diagnostics.log_warn(program)

def find_project_path(project : str, config : str, compiler : str, program : str):
  """Find the path of the program for the project specified using the config and compiler specified.
  Possible to specify the program name you want to launch in case there are multiple."""
  build_projects = regis.rex_json.load_file(build_projects_path)
  
  project = project.lower()
  config = config.lower()
  compiler = compiler.lower()

  # We need to make sure that the project with the config and compiler exists.
  # It's possible a launch request comes in for something that doesn't exist yet.
  # We need to capture these cases here and report something to the user when it doesn't exist.
  if project not in build_projects:
    regis.diagnostics.log_err(f'"{project}" has not been build.')
    return

  build_project = build_projects[project]
  if config not in build_project:
    regis.diagnostics.log_err(f'"{project} - {config}" has not been build.')
    return

  build_config = build_project[config]
  if compiler not in build_config:
    regis.diagnostics.log_err(f'"{project} - {config} - {compiler}" has not been build.')
    return

  paths : list[str] = build_projects[project][config][compiler] 

  if len(paths) == 0:
    regis.diagnostics.log_warn(f'{project} - {config} - {compiler} hasn\'t been build yet')
    return None

  # It's possible an executable has a dependency on another executable.
  # If that happens we have more than 1 exe to run
  # When that happens, the user needs to specify which program they want to run.
  if len(paths) > 1 and program == "":
      regis.diagnostics.log_warn(f'multiple programs were found for "{project} - {config} - {compiler}".')
      regis.diagnostics.log_warn(f'Please specify which program you want to launch with "-program" argument')
      display_program_options(paths)
      return None

  # Check if the user's program is found in the different paths that are found.
  # Warn if it's not found and early out
  if program != "":
    for path in paths:
      program_name = pathlib.Path(path).name
      if program_name.lower() == program.lower():
        return path

      regis.diagnostics.log_warn(f'no program was found with the name "{program}" for "{project} - {config} - {compiler}"')
      display_program_options(paths)
      return None

  # In all other cases (only 1 path found and no program specified)
  # we return the first path which is just the executable generated for the project
  return paths[0]

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)

  parser.add_argument("-project", default="regina", help="project to launch")
  parser.add_argument("-config", default="debug_opt", help="config the project was compiled in")
  parser.add_argument("-compiler", default="msvc", help="compiler that was used to compile the project")
  parser.add_argument("-program", default="", help="launch a specific program. If only one is found for this is target, that's launched by default.")
  parser.add_argument("-exe_args", default=[], action='append', help="The arguments to pass to the exe")

  args, unknown = parser.parse_known_args()

  project = args.project
  config = args.config
  compiler = args.compiler
  program = args.program

  exe_path = find_project_path(project, config, compiler, program)

  if not exe_path :
    regis.diagnostics.log_err(f'failed to find a program for "{project} - {config} - {compiler}"')
    if program:
      regis.diagnostics.log_err(f'while looking for "{program}"')
    result = 1
  else:
    regis.diagnostics.log_info(f'launching {exe_path} {args.exe_args}')

    exe_args = []
    exe_args.append(exe_path)
    exe_args.extend(args.exe_args)
    proc = subprocess.Popen(exe_args)

    proc.communicate()
    result = proc.returncode

  sys.exit(result)
