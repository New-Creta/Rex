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
import regis.test
import regis.diagnostics
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
    regis.test.clean()

  if args.all or args.iwyu:
    regis.test.test_include_what_you_use()
  if args.all or args.clang_tidy:
    regis.test.test_clang_tidy()
  if args.all or args.unit_tests:
    regis.test.test_unit_tests()
  if args.all or args.coverage:
    regis.test.test_code_coverage()
  if args.all or args.asan:
    regis.test.test_asan()
  if args.all or args.ubsan:
    regis.test.test_ubsan()
  if args.all or args.fuzzy:
    regis.test.test_fuzzy_testing()

  regis.diagnostics.log_no_color("")
  regis.diagnostics.log_info("Summary Report")
  regis.diagnostics.log_no_color("--------------------------------------")

  pass_results = regis.test.get_pass_results()
  for key in pass_results:
    result = pass_results[key]

    if result == 0:
      regis.diagnostics.log_info(f"{key} - success")
    else:
      regis.diagnostics.log_err(f"{key} - failed")

  end = time.perf_counter()
  regis.diagnostics.log_no_color("")
  regis.diagnostics.log_no_color("--------------------------------------")
  regis.diagnostics.log_info(f"Finished at: {datetime.now().strftime('%d %B %Y - %H:%M:%S %p')}")
  regis.diagnostics.log_info(f"Tests took {end - start:0.4f} seconds")
  