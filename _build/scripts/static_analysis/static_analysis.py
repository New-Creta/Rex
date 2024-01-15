import os
import re
import regis.util
import regis.diagnostics

source_root_folders = [
    'source',
    'tests'
]

exclude_folders = [
    '0_thirdparty',
    'catch2'
]
   
class FoundMatch():
    def __init__(self, filepath : str, line : str, line_nr : int, startPos : int):
        self.filepath = filepath
        self.line = line
        self.line_nr = line_nr
        self.start_pos = startPos

def scan_root(regex : str, sourceRoot : str, ignoreFolders : str):
    found_matches : list[FoundMatch] = []

    for root, dirs, files in os.walk(os.path.join(regis.util.find_root(), sourceRoot)):
        for exclude_folder in ignoreFolders:
            if exclude_folder in dirs:
                dirs.remove(exclude_folder)

        for file in files:
            file_path = os.path.join(root, file)
            with open(file_path) as f:
                try:
                    for line_number, line in enumerate(f, start=1):
                        matches = re.finditer(regex, line)
                        for match in matches:
                            found_matches.append(FoundMatch(file_path, line, line_number, match.start()))
                except Exception as ex:
                    regis.diagnostics.log_debug(f'failed to parse {file_path}')

    return found_matches

def scan_clang_tidy_ignore(sourceRoots : list[str], excludeFolders : list[str]):
    regis.diagnostics.log_info(f'scanning for clang-tidy ignore messages')
    regex_pattern = r'\/\/\s*NOLINT\(' # scanning for "// NOLINT("
    found_matches : list[FoundMatch] = []
    for source_root in sourceRoots:
        found_matches.extend(scan_root(regex_pattern, source_root, excludeFolders))

    ignored_warnings = {}

    for found_match in found_matches:
        ignore_text = found_match.line[found_match.start_pos:]
        start = ignore_text.find('(')+1
        end = ignore_text.find(')')
        warnings_str = ignore_text[start:end]
        warnings = warnings_str.split(',')
        for warning in warnings:
            warning = warning.lstrip()
            warning = warning.rstrip()
            if warning not in ignored_warnings:
                ignored_warnings[warning] = 0
            ignored_warnings[warning] += 1

    for warning in ignored_warnings:
        regis.diagnostics.log_info(f'{warning}: {ignored_warnings[warning]}')

def scan_std_usage(sourceRoots : list[str], excludeFolders : list[str]):
    regex_pattern = r'\bstd::\w+\b' # scanning for " std::"
    found_matches : list[FoundMatch] = []
    
    for source_root in sourceRoots:
        found_matches.extend(scan_root(regex_pattern, source_root, excludeFolders))

    for found_match in found_matches:
        regis.diagnostics.log_info(f'{found_match.filepath}({found_match.line_nr}): {found_match.line.rstrip()}')

def log_header(msg : str):
    regis.diagnostics.log_info('------------------------')
    regis.diagnostics.log_info(msg)
    regis.diagnostics.log_info('------------------------')

def main():
    log_header(f'scanning for clang-tidy ignores')
    scan_clang_tidy_ignore(source_root_folders, exclude_folders)
    log_header(f'scanning for std usages')
    scan_std_usage(source_root_folders, exclude_folders)

if __name__ == '__main__':
    main()