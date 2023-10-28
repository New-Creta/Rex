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
# it supports setting the compiler, configuration as well as the project to build.

import os
import sys
import argparse
import regis.build
import regis.util
import regis.diagnostics
import regis.task_raii_printing
import regis.dir_watcher
import regis.rex_json

root = regis.util.find_root()  
settings_path = os.path.join(root, "_build", "config", "settings.json")
settings = regis.rex_json.load_file(settings_path)
intermediate_path = os.path.join(root, settings['intermediate_folder'], settings['build_folder'])
build_projects_path = os.path.join(intermediate_path, settings['build_projects_filename'])

def update_cleaned_projects(project : str, config : str, compiler : str, deletedPrograms : list[str]):
  build_projects = regis.rex_json.load_file(build_projects_path)

  if project not in build_projects:
    return
  
  build_project = build_projects[project]
  if config not in build_project:
    return
  
  build_config = build_projects[project][config]
  if compiler not in build_config:
    return
  
  build_programs : list[str] = build_projects[project][config][compiler]

  for deleted_program in deletedPrograms:
    if deleted_program in build_programs:
      build_programs.remove(deleted_program)

  regis.rex_json.save_file(build_projects_path, build_projects)

def update_build_projects(project : str, config : str, compiler : str, paths : list[str]):
  build_projects = regis.rex_json.load_file(build_projects_path)

  if project not in build_projects:
    build_projects[project] = {}
  
  build_project = build_projects[project]
  if config not in build_project:
    build_project[config] = {}
    
  build_projects[project][config][compiler] = paths

  regis.rex_json.save_file(build_projects_path, build_projects)

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  
  # There a couple ways of calling this script, but in essence it's very simple
  # build.py                 <-- builds the project
  # build.py -clean          <-- rebuilds the project
  # build.py -nobuild -clean <-- cleans the project
  parser.add_argument("-nobuild", help="Don't perform a build of the project", action="store_true")
  parser.add_argument("-clean", help="clean all intermediate files", action="store_true")
  parser.add_argument("-sln", default="", help="path to nsln file")
  parser.add_argument("-project", default="regina", help="project to build")
  parser.add_argument("-config", default="debug_opt", help="configuration to build for")
  parser.add_argument("-compiler", default="unknown", help="configuration to build with")
  parser.add_argument("-dont_build_dependencies", default=False, help="build only the project specified and not its dependencies", action="store_true")

  args, unknown = parser.parse_known_args()

  if args.compiler == "unknown":
    if regis.util.is_windows():
      args.compiler = "msvc"
    else:
      args.compiler = "clang"

  with regis.dir_watcher.DirWatcher(intermediate_path, bRecursive=True) as dir_watcher:
    task = regis.task_raii_printing.TaskRaiiPrint("Building")
    result = regis.build.new_build(args.project, args.config, args.compiler, not args.nobuild, args.clean, args.sln, not args.dont_build_dependencies)

  if not os.path.exists(build_projects_path):
    regis.rex_json.save_file(build_projects_path, {})

  programs_deleted = dir_watcher.filter_deleted_files(lambda path: path.endswith('.exe'))
  programs_created = dir_watcher.filter_created_or_modified_files(lambda path: path.endswith('.exe'))

  update_cleaned_projects(args.project, args.config, args.compiler, programs_deleted)
  update_build_projects(args.project, args.config, args.compiler, programs_created)

  if result != 0:
    regis.diagnostics.log_err("Build failed")
  else:
    regis.diagnostics.log_info("Build successful")
  
  sys.exit(result)
