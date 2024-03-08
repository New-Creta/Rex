import regis.run_clang_tools
import regis.diagnostics
import argparse
import os
import sys

def run(projectName, compdb, srcroot, useClangTools, allChecks, clangTidyRegex):
  if (useClangTools):
    regis.run_clang_tools.run(projectName, compdb, srcroot, allChecks, clangTidyRegex, False)

  return

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)

  parser.add_argument("-p", "--project", help="project name")
  parser.add_argument("-comp", "--compiler", help="compiler")
  parser.add_argument("-conf", "--config", help="configuration")
  parser.add_argument("-compdb", help="compiler db folder")
  parser.add_argument("-srcroot", help="src root folder")
  parser.add_argument("-use_clang_tools", help="use clang tools", action="store_true")
  parser.add_argument("-perform_all_checks", help="perform all checks, not just the auto fixes", action="store_true")
  parser.add_argument("-clang_tidy_regex", help="regex to match files used by clang tidy", default=".*")
  parser.add_argument("-l", "--level", default="info", help="logging level")
  args, unknown = parser.parse_known_args()

  project_name = args.project
  compiler = args.compiler
  config = args.config
  compdb = args.compdb
  srcroot = args.srcroot
  use_clang_tools = args.use_clang_tools
  perform_all_checks = args.perform_all_checks
  clang_tidy_regex = args.clang_tidy_regex

 # initialize the logger
  log_level_str = args.level
  log_level = regis.diagnostics.logging_level_from_string(log_level_str)
  logger = regis.diagnostics.StreamLogger("setup", log_level)

 # useful for debugging
  logger.info(f"Executing {__file__}")

 # execute the script
  run(project_name, compdb, srcroot, use_clang_tools, perform_all_checks, clang_tidy_regex)

 # user hook
  project_post_build_script = os.path.join(srcroot, 'post_build.py')
  if os.path.isfile(project_post_build_script):
    arguments = ' '.join(sys.argv[1:]) # skip the first arguments as that's the path to this script
    logger.info(f'running project post build "{project_post_build_script}"')
    logger.info(f'with args: {arguments}')
    os.system(f'py {project_post_build_script} {arguments}')

 # print. We're done.
  logger.info("Done.")

  exit(0)
