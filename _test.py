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
import time
import threading
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
  parser.add_argument("-auto_test", help="run auto tests", action="store_true")
  parser.add_argument("-auto_test_timeout", help="timeout for auto tests in seconds", default=10)
  args,unknown = parser.parse_known_args()

  threads : list[threading.Thread] = []

  start = time.perf_counter()

  if args.clean:
    regis.test.clean()

  if args.all or args.iwyu:
    # threads.append(threading.Thread(target=lambda: regis.test.test_include_what_you_use()))
    regis.test.test_include_what_you_use()
  if args.all or args.clang_tidy:
    # threads.append(threading.Thread(target=lambda: regis.test.test_clang_tidy()))
    regis.test.test_clang_tidy()
  if args.all or args.unit_tests:
    # threads.append(threading.Thread(target=lambda: regis.test.test_unit_tests()))
    regis.test.test_unit_tests()
  if args.all or args.coverage:
    # threads.append(threading.Thread(target=lambda: regis.test.test_code_coverage()))
    regis.test.test_code_coverage()
  if args.all or args.asan:
    # threads.append(threading.Thread(target=lambda: regis.test.test_asan()))
    regis.test.test_asan()
  if args.all or args.ubsan:
    # threads.append(threading.Thread(target=lambda: regis.test.test_ubsan()))
    regis.test.test_ubsan()
  if args.all or args.fuzzy:
    # threads.append(threading.Thread(target=lambda: regis.test.test_fuzzy_testing()))
    regis.test.test_fuzzy_testing()
  if args.all or args.auto_test:
    if args.auto_test_timeout:
      auto_test_timeout_secs = args.auto_test_timeout

    # threads.append(threading.Thread(target=lambda: regis.test.run_auto_tests(int(auto_test_timeout_secs))))
    regis.test.run_auto_tests(int(auto_test_timeout_secs))

  for thread in threads:
    thread.start()

  for thread in threads:
    thread.join()

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