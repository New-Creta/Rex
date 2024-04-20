# This script is to test you're all setup and have the necessary files to start development

import os
import sys
import argparse

try:
  import regis
except:
  this_file_dir = os.path.dirname(__file__)
  root = os.path.abspath(os.path.join(this_file_dir, os.path.pardir, os.path.pardir))
  print(f'ERROR: regis not installed, please run "py _rex.py setup" in the root directory. (root directory: "{os.path.abspath(__file__)}")')
  sys.exit(1)

import regis.diagnostics
import regis.required_tools
import regis.required_libs
import regis.required_externals
import regis.util
import regis.git

def _validate_file_present(filepath : str, missingFiles : list[str]):
  if not os.path.isfile(filepath):
    missingFiles.append(filepath)
    return False
  
  return True

def _validate_dir_present(dirpath : str, missingDirs : list[str]):
   if not os.path.isdir(dirpath):
      missingDirs.append(dirpath)
      return False
   
   return True

def _validate_sharpmake_scripts(root : str):
  regis.diagnostics.log_info('checking if sharpmake scripts are installed')
  
  missing_sharpmake_files = []

  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'data', 'default_config.json'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'base.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'clang_tools_project.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'code_generation.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'config.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'generate_settings.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'globals.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'main.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'sharpmake.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'sln.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'target.cs'), missing_sharpmake_files)
  _validate_file_present(os.path.join(root, '_build', 'sharpmake', 'src', 'utils.cs'), missing_sharpmake_files)

  if len(missing_sharpmake_files) > 0:
      regis.diagnostics.log_err(f'You have missing sharpmake files')
      for file in missing_sharpmake_files:
          regis.diagnostics.log_err(f'missing sharpmake file: "{file}"')
  else:
     regis.diagnostics.log_info('all required sharpmake scripts found.')

  return missing_sharpmake_files

def _validate_rex_pipeline_scripts(root : str):
   regis.diagnostics.log_info('checking if pipeline scripts are installed')
   missing_pipeline_scripts = []

   _validate_file_present(os.path.join(root, '_rex.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'auto_test.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'build.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'code_analysis.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'fuzzy_test.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'generate.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'launch.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'validate.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'setup.py'), missing_pipeline_scripts)
   _validate_file_present(os.path.join(root, '_build', 'scripts', 'unit_test.py'), missing_pipeline_scripts)

   if len(missing_pipeline_scripts) > 0:
      regis.diagnostics.log_err('You have missing pipeline scripts')
      for file in missing_pipeline_scripts:
         regis.diagnostics.log_err(f'missing pipeline script: "{file}"')
   else:
     regis.diagnostics.log_info('all required pipeline scripts found.')

   return missing_pipeline_scripts

def _validate_rex_sources(root : str):
  regis.diagnostics.log_info('checking if rex sources synced')
  missing_dirs = []

  if _validate_dir_present(os.path.join(root, 'source'), missing_dirs):
     _validate_dir_present(os.path.join(root, 'source', '0_thirdparty'), missing_dirs)
     _validate_dir_present(os.path.join(root, 'source', '1_engine'), missing_dirs)
     _validate_dir_present(os.path.join(root, 'source', '2d_platform'), missing_dirs)
     _validate_dir_present(os.path.join(root, 'source', '4_tools'), missing_dirs)
  if _validate_dir_present(os.path.join(root, 'tests'), missing_dirs):
     _validate_dir_present(os.path.join(root, 'tests', '1_engine'), missing_dirs)
     _validate_dir_present(os.path.join(root, 'tests', '4_tools'), missing_dirs)

  if len(missing_dirs) > 0:
      regis.diagnostics.log_err('You have missing rex directories')
      for dir in missing_dirs:
         regis.diagnostics.log_err(f'missing directory script: "{dir}"')
  else:
    regis.diagnostics.log_info('all required rex sources found.')

  return missing_dirs

def _validate(promptAutoFix : bool):
  auto_fix_funcs = []
  invalid_setup = False

  # check if tools are installed
  if not regis.required_tools.query():
     regis.diagnostics.log_err(f'Not all tools are installed')
     auto_fix_funcs.append(regis.required_tools.install)
     invalid_setup = True

  # check if libs are installed
  if not regis.required_libs.query():
     regis.diagnostics.log_err(f'Not all libs are installed')
     auto_fix_funcs.append(regis.required_libs.install)
     invalid_setup = True

  # check if externals are installed
  if not regis.required_externals.query():
     regis.diagnostics.log_err(f'Not all externals are installed')
     auto_fix_funcs.append(regis.required_externals.install)
     invalid_setup = True

  # check if we have the required sharpmake scripts
  root = regis.util.find_root()
  missing_sharpmake_files = _validate_sharpmake_scripts(root)

  # check if you have the scripts to run the rex pipeline
  missing_pipeline_scripts = _validate_rex_pipeline_scripts(root)

  # check if you have the engine projects
  missing_source_dirs = _validate_rex_sources(root)

  if len(missing_sharpmake_files) > 0 or len(missing_pipeline_scripts) > 0 or len(missing_source_dirs) > 0:
    regis.diagnostics.log_err(f'Not all git files are pulled')
    auto_fix_funcs.append(regis.git.pull)
    invalid_setup = True
  
  if invalid_setup and promptAutoFix:
    should_auto_fix = regis.util.ask_yesno('Some error were encountered, do you want to attempt to auto fix?')
    
    # attempt to auto fix and rerun validation
    if should_auto_fix:
      for func in auto_fix_funcs:
        func()
      
      regis.diagnostics.log_info('Revalidating after auto fix..')
      return _validate(promptAutoFix=False)

  if invalid_setup:
     regis.diagnostics.log_err(f'Invalid setup, please investigate.')
  else:
     regis.diagnostics.log_info('All setup validation checks passed.')

  return invalid_setup
      
def main():
  has_invalid_setup = _validate(promptAutoFix=True)
  if has_invalid_setup:
    sys.exit(1)
  else:
    sys.exit(0)

if __name__ == "__main__":
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  args, unknown = parser.parse_known_args()
  
  main()