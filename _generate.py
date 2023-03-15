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
  parser.add_argument("-no_clang_tools", help="don't generate a compiler db", action="store_true")
  parser.add_argument("-unittests", help="generate unit tests", action="store_true")
  parser.add_argument("-coverage", help="generate coverage", action="store_true")
  parser.add_argument("-asan", help="generate address sanitizer", action="store_true")
  parser.add_argument("-ubsan", help="generate undefined behavior sanitizer", action="store_true")
  parser.add_argument("-fuzzy", help="generate fuzzy testing", action="store_true")

  args, unknown = parser.parse_known_args()

  root = regis.util.find_root()
  settings_path = os.path.join(root, "build", "config", "settings.json")

  run_any_tests = args.unittests or args.coverage or args.asan or args.ubsan or args.fuzzy

  sharpmake_args = ""
  if args.no_clang_tools:
    sharpmake_args += " /noClangTools"

  if args.unittests:
    sharpmake_args += " /generateUnitTests"

  elif args.coverage:
    sharpmake_args += " /generateUnitTests /enableCoverage"

  elif args.asan:
    sharpmake_args += " /generateUnitTests /enableAddressSanitizer"

  elif args.ubsan:
    sharpmake_args += " /generateUnitTests /enableUBSanitizer"

  elif args.fuzzy:
    sharpmake_args += " /enableFuzzyTesting"

  result = 0

  proc = regis.generation.new_generation(settings_path, sharpmake_args)
  proc.wait()
  result = proc.returncode

  exit(result)
