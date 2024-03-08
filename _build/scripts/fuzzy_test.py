import argparse
import os
import time
import regis.test
import regis.diagnostics
import regis.util
from datetime import datetime

root_path = regis.util.find_root()
settings = regis.rex_json.load_file(os.path.join(root_path, regis.util.settingsPathFromRoot))

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument("-clean", help="clean run, as if run for the first time", action="store_true")
  parser.add_argument("-build_single_threaded", help="build tests in single threaded mode", action="store_true")
  parser.add_argument("-runs", help="num runs to perform", default=10000)
  parser.add_argument("-enable-asan", help="enable asan when running the tests", default=False, action="store_true")
  parser.add_argument("-enable-ubsan", help="enable ubsan when running the tests", default=False, action="store_true")
  parser.add_argument("-enable-coverage", help="enable code coverage when running the tests", default=False, action="store_true")

  parser.add_argument("-project", dest="projects", help="Append a project to run a test on. Leave empty to run all projects applicable", action="append", default=[])
  args,unknown = parser.parse_known_args()

  start = time.perf_counter()

  regis.test.test_fuzzy_testing(args.projects, args.runs, args.clean, args.build_single_threaded, args.enable_asan, args.enable_ubsan, args.enable_coverage)

  regis.diagnostics.log_no_color("")
  regis.diagnostics.log_info("Summary Report")
  regis.diagnostics.log_no_color("--------------------------------------")

  end_result = 0
  pass_results = regis.test.get_pass_results()
  for key in pass_results:
    result = pass_results[key]
    end_result |= result

    if result == 0:
      regis.diagnostics.log_info(f"{key} - success")
    else:
      regis.diagnostics.log_err(f"{key} - failed")

  end = time.perf_counter()
  regis.diagnostics.log_no_color("")
  regis.diagnostics.log_no_color("--------------------------------------")
  regis.diagnostics.log_info(f"Finished at: {datetime.now().strftime('%d %B %Y - %H:%M:%S %p')}")
  regis.diagnostics.log_info(f"Tests took {end - start:0.4f} seconds")
  
  exit(end_result)