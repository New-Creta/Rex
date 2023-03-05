# ============================================ 
#
# Author: Nick De Breuck
# Twitter: @nick_debreuck
# 
# File: generate.py
# Copyright (c) Nick De Breuck 2023
#
# ============================================

import os
import argparse
import regis.generation
import regis.rex_json
import regis.util
import glob
from pathlib import Path

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument("-unittests", help="generate unit tests", action="store_true")
  parser.add_argument("-coverage", help="generate coverage", action="store_true")
  parser.add_argument("-asan", help="generate address sanitizer", action="store_true")
  parser.add_argument("-ubsan", help="generate undefined behavior sanitizer", action="store_true")
  parser.add_argument("-fuzzy", help="generate fuzzy testing", action="store_true")

  args, unknown = parser.parse_known_args()

  root = regis.util.find_root()
  settings_path = os.path.join(root, "build", "config", "settings.json")

  run_any_tests = args.unittests or args.coverage or args.asan or args.ubsan or args.fuzzy

  if run_any_tests == False:
    regis.generation.new_generation(settings_path, "")

  if args.unittests:
    regis.generation.new_generation(settings_path, "/generateUnitTests")
  
  if args.coverage:
    regis.generation.new_generation(settings_path, "/generateUnitTests /enableCoverage")

  if args.asan:
    regis.generation.new_generation(settings_path, "/generateUnitTests /enableAddressSanitizer")

  if args.ubsan:
    regis.generation.new_generation(settings_path, "/generateUnitTests /enableUBSanitizer")

  if args.fuzzy:
    regis.generation.new_generation(settings_path, "/enableFuzzyTesting")
