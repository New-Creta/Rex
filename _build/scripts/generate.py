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

# TODO: Somehow make it happen that these arguments are like plugins
# that way the maintenance of them is taken out of sharpmake so any script
# or application can talk to them and these arguments don't need to be just known
# Avoiding duplication and making it more scalable.

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

  sharpmake_args = "/enableVisualStudio /disableClangTidyForThirdParty"

  # If a test is specified, look up its arguments and add them
  # to the arguments we'll pass to sharpmake
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

  # Apparently we cannot pass double quotes as an argument to python
  # At least by my knowledge ... 
  sharpmake_args = sharpmake_args.replace("\'", "\"")
  # We also need to make sure we use forward slashes.
  sharpmake_args = sharpmake_args.replace("\\", "\/")

  root = regis.util.find_root()
  settings_path = os.path.join(root, "_build", "config", "settings.json")

  # To avoid duplicating code in the toolchain and to avoid hardcoded paths
  # as much as possible, we save most paths, extensions and other hardcoded values
  # into a settings file.
  # this way multiple script can use these settings, including the engine itself.
  proc = regis.generation.new_generation(settings_path, sharpmake_args)
  proc.wait()
  result = proc.returncode

  exit(result)
