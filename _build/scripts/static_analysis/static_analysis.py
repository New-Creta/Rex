import os
import re
from pathlib import Path
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

def _is_sharpmake_file(filepath : Path):
    if len(filepath.suffixes) != 2:
        return False
    
    return filepath.suffixes[0] == '.sharpmake' and filepath.suffixes[1] == '.cs'

def _is_correct_include_filepath(filepath : Path):
    file_path_splitted = filepath.__str__().split(os.path.sep)
    return 'include' in file_path_splitted

def _is_correct_src_filepath(filepath : str):
    file_path_splitted = filepath.__str__().split(os.path.sep)
    return 'src' in file_path_splitted

class FilePathMetaData:
    def __init__(self, filepath : Path):
        splitted_path = filepath.__str__().split(os.path.sep)

        self.root = ""
        self.project_name = ""
        self.dir_in_project = ""
        self.filename = ""

        if 'src' in splitted_path:
            self._init_from_src(filepath)
        elif 'include' in splitted_path:
            self._init_from_header(filepath)

    def _init_from_src(self, filepath : Path):
        splitted_path = filepath.__str__().split(os.path.sep)
        src_folder_idx = splitted_path.index('src')
        project_name_idx = src_folder_idx - 1

        self.root = os.path.sep.join(splitted_path[:project_name_idx])
        self.project_name = splitted_path[project_name_idx]
        self.dir_in_project = os.path.sep.join(splitted_path[src_folder_idx + 1:-1])
        self.filename = splitted_path[-1]
    
    def _init_from_header(self, filepath : Path):
        splitted_path = filepath.__str__().split(os.path.sep)
        include_folder_idx = splitted_path.index('include')
        project_name_idx = include_folder_idx - 1

        self.root = os.path.sep.join(splitted_path[:project_name_idx])
        self.project_name = splitted_path[project_name_idx]
        self.dir_in_project = os.path.sep.join(splitted_path[include_folder_idx + 2:-1])
        self.filename = splitted_path[-1]

def _expected_header_file_for_cpp(filepath : Path):
    filepath_metadata = FilePathMetaData(filepath)

    root = filepath_metadata.root
    project_name = filepath_metadata.project_name
    dir_in_project = filepath_metadata.dir_in_project
    filename = filepath_metadata.filename
    header_filename = filename.replace(".cpp", ".h")

    return os.path.join(root, project_name, "include", project_name, dir_in_project, header_filename)

def _expected_cpp_file_for_header(filepath : Path):
    filepath_metadata = FilePathMetaData(filepath)

    root = filepath_metadata.root
    project_name = filepath_metadata.project_name
    dir_in_project = filepath_metadata.dir_in_project
    filename = filepath_metadata.filename
    src_filename = filename.replace(".h", ".cpp")

    return os.path.join(root, project_name, "src", dir_in_project, src_filename)

def _find_matching_header_file(filepath : Path):
    # d:\project\src\folder\source.cpp <-> d:\project\include\project\folder\source.h
    expected_header_filepath = _expected_header_file_for_cpp(filepath)

    if os.path.isfile(expected_header_filepath):
        return Path(expected_header_filepath)

    filepath_metadata = FilePathMetaData(filepath)
    root = filepath_metadata.root
    project_name = filepath_metadata.project_name
    header_filename = filepath_metadata.filename.replace(".cpp", ".h")

    # It's possibe there's no header for this file. However, we need to make sure that the header for this file isn't located elsewhere
    project_include_path = os.path.join(root, project_name, "include", project_name)
    for root, dirs, files in os.walk(project_include_path):
        for file in files:
            full_filepath = Path(os.path.join(root, file))
            if full_filepath.name == header_filename:
                return full_filepath
            
    return None

def _find_matching_cpp_file(filepath : Path):
    # d:\project\src\folder\source.cpp <-> d:\project\include\project\folder\source.h
    expected_src_filepath = _expected_cpp_file_for_header(filepath)

    if os.path.isfile(expected_src_filepath):
        return Path(expected_src_filepath)
    
    filepath_metadata = FilePathMetaData(filepath)
    root = filepath_metadata.root
    project_name = filepath_metadata.project_name
    src_filename = filepath_metadata.filename.replace(".h", ".cpp")

    # It's possibe there's no header for this file. However, we need to make sure that the header for this file isn't located elsewhere
    project_src_path = os.path.join(root, project_name, "src")
    for root, dirs, files in os.walk(project_src_path):
        for file in files:
            full_filepath = Path(os.path.join(root, file))
            if full_filepath.name == src_filename:
                return full_filepath
            
    return None

def _does_source_and_header_match(sourceFilePath : Path, headerFilePath : Path):
    """Headers file may have source files. If they do, they should follow
    the same structure as where the header is located"""
    
    # d:\project\src\folder\source.cpp <-> d:\project\include\project\folder\source.h
    absolute_source_filepath = sourceFilePath.absolute().__str__()
    absolute_header_filepath = headerFilePath.absolute().__str__()

    expected_header_filepath = _expected_header_file_for_cpp(sourceFilePath)
    expected_source_filepath = _expected_cpp_file_for_header(headerFilePath)

    return expected_header_filepath == absolute_header_filepath and expected_source_filepath == absolute_source_filepath

def scan_incorrect_source_file_locations(sourceRoots : list[str], ignoreFolders : list[str]):
    num_incorrect_sources_found = 0

    for source_root in sourceRoots:
        for root, dirs, files in os.walk(source_root):
            for ignore_folder in ignoreFolders:
                if ignore_folder in dirs:
                    dirs.remove(ignore_folder)

            for file in files:
                filepath = Path(os.path.join(root, file))
                if _is_sharpmake_file(filepath):
                    continue

                if len(filepath.suffixes) > 1:
                    regis.diagnostics.log_warn(f'more than 1 extension used for filepath "{filepath}"')
                    num_incorrect_sources_found += 1
                    continue

                if (filepath.suffix == '.h'):
                    if not _is_correct_include_filepath(filepath):
                        regis.diagnostics.log_warn(f'header file found in incorrect location "{filepath}"')
                        num_incorrect_sources_found += 1
                        continue

                    matching_src_filepath = _find_matching_cpp_file(filepath)
                    if matching_src_filepath and not _does_source_and_header_match(matching_src_filepath, filepath):
                        regis.diagnostics.log_warn(f'Unexpected source file found for header "{filepath.absolute()}". Source file: "{matching_src_filepath}"')
                        regis.diagnostics.log_warn(f'Expected to be at: {_expected_cpp_file_for_header(filepath)}')
                        num_incorrect_sources_found += 1
                        continue

                if (filepath.suffix == '.cpp'):
                    if not _is_correct_src_filepath(filepath):
                        regis.diagnostics.log_warn(f'source file found in incorrect location "{filepath}"')
                        num_incorrect_sources_found += 1
                        continue

                    matching_header_filepath = _find_matching_header_file(filepath)
                    if matching_header_filepath and not _does_source_and_header_match(filepath, matching_header_filepath):
                        regis.diagnostics.log_warn(f'Unexpected header file found for source "{filepath.absolute()}". Header file: "{matching_header_filepath}"')
                        regis.diagnostics.log_warn(f'Expected to be at: {_expected_header_file_for_cpp(filepath)}')
                        num_incorrect_sources_found += 1
                        continue
                
    regis.diagnostics.log_info(f'found {num_incorrect_sources_found} incorrect sources')
                  
def log_header(msg : str):
    regis.diagnostics.log_info('------------------------')
    regis.diagnostics.log_info(msg)
    regis.diagnostics.log_info('------------------------')

def main():
    absolute_source_root = []
    root = regis.util.find_root()
    for source_root in source_root_folders:
        absolute_source_root.append(os.path.join(root, source_root))        

    log_header('scanning for clang-tidy ignores')
    scan_clang_tidy_ignore(absolute_source_root, exclude_folders)
    log_header('scanning for std usages')
    scan_std_usage(absolute_source_root, exclude_folders)
    log_header('scanning for incorrect source file locations')
    scan_incorrect_source_file_locations(absolute_source_root, exclude_folders)

if __name__ == '__main__':
    main()