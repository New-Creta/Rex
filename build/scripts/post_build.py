import run_clang_tools
import diagnostics
import argparse

def run(projectName, config):
  # run_clang_tools.run(projectName, config)
  return

def main():
  parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)

  parser.add_argument("-p", "--project", help="project name")
  parser.add_argument("-comp", "--compiler", help="compiler")
  parser.add_argument("-conf", "--config", help="configuration")

  parser.add_argument("-l", "--level", default="info", help="logging level")
  args, unknown = parser.parse_known_args()

  project_name = args.project
  compiler = args.compiler
  config = args.config

 # initialize the logger
  log_level_str = args.level
  log_level = diagnostics.logging_level_from_string(log_level_str)
  logger = diagnostics.StreamLogger("setup", log_level)

 # useful for debugging
  logger.info(f"Executing {__file__}")

 # execute the script
  run(project_name, config)

 # print. We're done.
  logger.info("Done.")

  exit(0)

if __name__ == "__main__":
  main()