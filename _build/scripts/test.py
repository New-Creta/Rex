# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: test.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

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
  parser.add_argument("-single_threaded", help="run tests in single threaded mode", action="store_true")
  parser.add_argument("-only_errors_and_warnings", help="filter lines to only display warnings and errors", action="store_true")
  parser.add_argument("-auto_fix", help="auto fix where you can (applies to iwyu and clang-tidy)", action="store_true")

  parser.add_argument("-project", dest="projects", help="Append a project to run a test on. Leave empty to run all projects applicable", action="append", default=[])

  parser.add_argument("-all", help="run all tests", action="store_true")
  parser.add_argument("-iwyu", help="run include-what-you-use", action="store_true")
  parser.add_argument("-clang_tidy", help="run clang-tidy", action="store_true")
  parser.add_argument("-unit_tests", help="run unit tests", action="store_true")
  parser.add_argument("-coverage", help="run coverage tests", action="store_true")
  parser.add_argument("-asan", help="run address sanitizer", action="store_true")
  parser.add_argument("-ubsan", help="run undefined behavior sanitizer", action="store_true")
  parser.add_argument("-fuzzy", help="run fuzzy testing", action="store_true")
  parser.add_argument("-auto_test", help="run auto tests", action="store_true")
  parser.add_argument("-auto_test_timeout", help="timeout for auto tests in seconds", default=10)
  args,unknown = parser.parse_known_args()

  start = time.perf_counter()

  if args.all or args.iwyu:
    regis.test.test_include_what_you_use(args.clean, args.single_threaded, args.auto_fix)
  if args.all or args.clang_tidy:
    regis.test.test_clang_tidy(".*", args.clean, args.single_threaded, args.only_errors_and_warnings, args.auto_fix)
  if args.all or args.unit_tests:
    regis.test.test_unit_tests(args.projects, args.clean, args.single_threaded)
  if args.all or args.coverage:
    regis.test.test_code_coverage(args.projects, args.clean, args.single_threaded)
  if args.all or args.asan:
    regis.test.test_asan(args.projects, args.clean, args.single_threaded)
  if args.all or args.ubsan:
    regis.test.test_ubsan(args.projects, args.clean, args.single_threaded)
  if args.all or args.fuzzy:
    regis.test.test_fuzzy_testing(args.projects, args.clean, args.single_threaded)
  if args.all or args.auto_test:
    if args.auto_test_timeout:
      auto_test_timeout_secs = args.auto_test_timeout

    regis.test.run_auto_tests(["debug", "debug_opt", "release"], ["msvc","clang"], args.projects, int(auto_test_timeout_secs), args.clean, args.single_threaded)

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