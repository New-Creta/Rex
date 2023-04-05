py _setup.py
py _generate.py
py _build.py -project=regina -compiler=msvc -config=debug
py _build.py -project=regina -compiler=msvc -config=debug_opt
py _build.py -project=regina -compiler=msvc -config=release
py _build.py -project=regina -compiler=clang -config=debug
py _build.py -project=regina -compiler=clang -config=debug_opt
py _build.py -project=regina -compiler=clang -config=release
@REM py _test.py -iwyu -clang_tidy -unit_tests -asan -ubsan