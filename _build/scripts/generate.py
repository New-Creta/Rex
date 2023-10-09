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
  parser.add_argument("-enabled_test", default='', help="Enable the solution to run these kind of tests")
  parser.add_argument("-no_clang_tools", help="don't generate a compiler db", action="store_true")
  parser.add_argument("-unit_tests", help="generate unit tests", action="store_true")
  parser.add_argument("-enable_second_pass", help="Enable second pass of clang-tidy checks", action="store_true")
  parser.add_argument("-coverage", help="generate coverage", action="store_true")
  parser.add_argument("-asan", help="generate address sanitizer", action="store_true")
  parser.add_argument("-ubsan", help="generate undefined behavior sanitizer", action="store_true")
  parser.add_argument("-fuzzy", help="generate fuzzy testing", action="store_true")
  parser.add_argument("-sharpmake_args", help="arguments passed to sharpmake")

  args, unknown = parser.parse_known_args()

  root = regis.util.find_root()
  settings_path = os.path.join(root, "_build", "config", "settings.json")

  run_any_tests = args.enabled_test
  run_any_tests = args.unit_tests or args.coverage or args.asan or args.ubsan or args.fuzzy

  sharpmake_args = "/enableVisualStudio /disableClangTidyForThirdParty"
  if args.no_clang_tools:
    sharpmake_args += " /noClangTools"

  if args.enable_second_pass:
    sharpmake_args += " /enableSecondPass"

  if "unit_tests" == args.enabled_test:
    sharpmake_args += " /generateUnitTests"

  elif "coverage" == args.enabled_test:
    sharpmake_args += " /generateUnitTests /enableCoverage"

  elif "asan" == args.enabled_test:
    sharpmake_args += " /generateUnitTests /enableAddressSanitizer"

  elif "ubsan" == args.enabled_test:
    sharpmake_args += " /generateUnitTests /enableUBSanitizer"

  elif "fuzzy" == args.enabled_test:
    sharpmake_args += " /enableFuzzyTesting"

  if args.sharpmake_args:
    sharpmake_args += f" {args.sharpmake_args}"

  result = 0

  # Apparantly we cannot pass doulbe quotes as an argument to python
  # At least by my knowledge ... 
  sharpmake_args = sharpmake_args.replace("\'", "\"")
  sharpmake_args = sharpmake_args.replace("\\", "\/")

  proc = regis.generation.new_generation(settings_path, sharpmake_args)
  proc.wait()
  result = proc.returncode

  exit(result)
