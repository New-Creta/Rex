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
import rexpy.generation
import rexpy.rex_json
import rexpy.util
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

  root = rexpy.util.find_root()
  settings_path = os.path.join(root, "build", "config", "settings.json")

  run_any_tests = args.unittests or args.coverage or args.asan or args.ubsan or args.fuzzy

  if run_any_tests == False:
    rexpy.generation.new_generation(settings_path, "")

  if args.unittests:
    rexpy.generation.new_generation(settings_path, "/generateUnitTests")
  
  if args.coverage:
    rexpy.generation.new_generation(settings_path, "/generateUnitTests /enableCoverage")

  if args.asan:
    rexpy.generation.new_generation(settings_path, "/generateUnitTests /enableAddressSanitizer")

  if args.ubsan:
    rexpy.generation.new_generation(settings_path, "/generateUnitTests /enableUBSanitizer")

  if args.fuzzy:
    rexpy.generation.new_generation(settings_path, "/enableFuzzyTesting")
