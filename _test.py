# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: test.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

import os
import argparse
import time
import rexpy.test
import rexpy.diagnostics
from datetime import datetime

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-clean", help="clean run, as if run for the first time", action="store_true")

  parser.add_argument("-all", help="run all tests", action="store_true")
  parser.add_argument("-iwyu", help="run include-what-you-use", action="store_true")
  parser.add_argument("-clang_tidy", help="run clang-tidy", action="store_true")
  parser.add_argument("-unit_tests", help="run unit tests", action="store_true")
  parser.add_argument("-coverage", help="run coverage tests", action="store_true")
  parser.add_argument("-asan", help="run address sanitizer", action="store_true")
  parser.add_argument("-ubsan", help="run undefined behavior sanitizer", action="store_true")
  parser.add_argument("-fuzzy", help="run fuzzy testing", action="store_true")
  
  args,unknown = parser.parse_known_args()

  start = time.perf_counter()

  if args.clean:
    rexpy.test.clean()

  if args.all or args.iwyu:
    rexpy.test.test_include_what_you_use()
  if args.all or args.clang_tidy:
    rexpy.test.test_clang_tidy()
  if args.all or args.unit_tests:
    rexpy.test.test_unit_tests()
  if args.all or args.coverage:
    rexpy.test.test_code_coverage()
  if args.all or args.asan:
    rexpy.test.test_asan()
  if args.all or args.ubsan:
    rexpy.test.test_ubsan()
  if args.all or args.fuzzy:
    rexpy.test.test_fuzzy_testing()

  rexpy.diagnostics.log_no_color("")
  rexpy.diagnostics.log_info("Summary Report")
  rexpy.diagnostics.log_no_color("--------------------------------------")

  pass_results = rexpy.test.get_pass_results()
  for key in pass_results:
    result = pass_results[key]

    if result == 0:
      rexpy.diagnostics.log_info(f"{key} - success")
    else:
      rexpy.diagnostics.log_err(f"{key} - failed")

  end = time.perf_counter()
  rexpy.diagnostics.log_no_color("")
  rexpy.diagnostics.log_no_color("--------------------------------------")
  rexpy.diagnostics.log_info(f"Finished at: {datetime.now().strftime('%d %B %Y - %H:%M:%S %p')}")
  rexpy.diagnostics.log_info(f"Tests took {end - start:0.4f} seconds")
  