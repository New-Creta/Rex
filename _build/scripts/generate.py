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
import regis.diagnostics
import glob
from pathlib import Path

supported_tests = {
  "unit_test" : " /generateUnitTests",
  "coverage" : " /generateUnitTests /enableCoverage",
  "asan" : " /generateUnitTests /enableAddressSanitizer",
  "ubsan" : " /generateUnitTests /enableUBSanitizer",
  "fuzzy" : " /enableFuzzyTesting",
}

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument("-enabled_test", default='', help=f"Specify the test to be enabled. ({list(supported_tests.keys())}) ")
  parser.add_argument("-no_clang_tools", help="don't generate a compiler db", action="store_true")
  parser.add_argument("-all_clangtidy_checks", help="Perform all configured clang tidy checks. not just the auto fixable ones", action="store_true")
  parser.add_argument("-sharpmake_args", help="Specify additional arguments to be passed to sharpmake")

  args, unknown = parser.parse_known_args()

  root = regis.util.find_root()
  settings_path = os.path.join(root, "_build", "config", "settings.json")

  sharpmake_args = "/enableVisualStudio /disableClangTidyForThirdParty"

  if args.enabled_test:
    if args.enabled_test in supported_tests:
      sharpmake_args += supported_tests[args.enabled_test]
    else:
      regis.diagnostics.log_err(f'"{args.enabled_test}" not found in supported test')
      regis.diagnostics.log_err(f'the supported tests are: {supported_tests.keys()}')
      exit(1)
  
  if args.no_clang_tools:
    sharpmake_args += " /noClangTools"

  if args.all_clangtidy_checks:
    sharpmake_args += " /PerformAllClangTidyChecks"

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
