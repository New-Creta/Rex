# This scripts takes a snapshot of what's currently in the engine and editor
# and saves this information to a manifest file

import os
import sys
import argparse
import regis.rex_json
import regis.util
import regis.diagnostics

_err_invalid_setup = 1
_err_invalid_result = 2

def add_to_manifest(manifest, key, paths):
    manifest[key] = []
    rex_root = regis.util.find_root()

    # remember to always save the path with '~' as root
    # as we cannot guarantee that rex is always saved under the same root
    # on someone else's machine
    for rooted_path in paths:
        full_path = rooted_path.replace('~', rex_root)
        if not os.path.exists(full_path):
            continue

        if os.path.isfile(full_path):
            manifest[key].append(rooted_path)
            continue

        if os.path.isdir(full_path):
            for root, dirs, files in os.walk(full_path):
                for file in files:
                    full_filepath = os.path.join(root, file)
                    full_filepath = full_filepath.replace(rex_root, '~')
                    manifest[key].append(full_filepath)

                continue

def generate():
    paths_file = os.path.join(os.path.dirname(__file__), 'paths.json')
    regis.diagnostics.log_info(f'loading: {paths_file}')
    json_blob = regis.rex_json.load_file(paths_file)
    if not json_blob:
        regis.diagnostics.log_err(f'Invalid paths file, stoping manifest generation')
        sys.exit(_err_invalid_setup)

    engine_paths : list[str] = json_blob['engine_paths']
    editor_paths : list[str] = json_blob['editor_paths']

    manifest_file = {}
    add_to_manifest(manifest_file, 'engine_paths', engine_paths)
    add_to_manifest(manifest_file, 'editor_paths', editor_paths)
 
    output_file = os.path.join(os.path.dirname(__file__), 'manifest.json')
    regis.diagnostics.log_info(f'saving: {output_file}')
    regis.rex_json.save_file(output_file, manifest_file)

def validate_paths(root : str, paths : list[str]):
    not_found_paths = []
    for path in paths:
        path = path.replace('~', root)
        if not os.path.isfile(path):
            not_found_paths.append(path)

    return not_found_paths

def validate(manifestPath : str):
    absolute_manifest_path = os.path.abspath(manifestPath)
    if not os.path.isfile(absolute_manifest_path):
        regis.diagnostics.log_err(f'No manifest file found at {manifestPath}')
        sys.exit(_err_invalid_setup)

    regis.diagnostics.log_info(f'Loading {absolute_manifest_path}')
    json_blob = regis.rex_json.load_file(absolute_manifest_path)
    if not json_blob:
        regis.diagnostics.log_err('Invalid manifest file. Halting validation')
        sys.exit(_err_invalid_setup)

    engine_paths : list[str] = json_blob['engine_paths']
    editor_paths : list[str] = json_blob['editor_paths']

    if not engine_paths:
        regis.diagnostics.log_warn('No engine paths found, is this intended?')

    if not editor_paths:
        regis.diagnostics.log_warn('No editor paths found, is this intended?')

    not_found_paths = []

    rex_root = regis.util.find_root(os.path.dirname(absolute_manifest_path))
    not_found_paths.extend(validate_paths(rex_root, engine_paths))
    not_found_paths.extend(validate_paths(rex_root, editor_paths))

    if not_found_paths:
        regis.diagnostics.log_err('Not all files in manifest are found')
        for path in not_found_paths:
            regis.diagnostics.log_err(f'Missing file: {path}')

        sys.exit(_err_invalid_result)
    else:
        regis.diagnostics.log_info('All files in manifest were found')

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-generate', help='Generate a snapshot from the current files', action='store_true', default=False)
    parser.add_argument('-validate', help='Validate a snapshot from a given manifest file', action='store_true', default=False)
    parser.add_argument('-manifest', help='Path to the manifest file. The root will be calculated from the manifest file', default='manifest.json')

    args, unknown = parser.parse_known_args()
    if args.generate and args.validate:
        regis.diagnostics.log_err('generate and validate option cannot both be true')
        sys.exit(_err_invalid_setup)

    if (args.generate or args.validate) == False:
        regis.diagnostics.log_err('neither generate nor validate option was specified')
        parser.print_help()
        sys.exit(_err_invalid_setup)

    if args.generate:
        generate()
        sys.exit(0)

    if args.validate:
        validate(args.manifest)
        sys.exit(0)


if __name__ == '__main__':
    main()