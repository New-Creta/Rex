#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/text_processing/ini.h"

#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/tmp_cwd.h"

TEST_CASE("TEST - Ini - with headers")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rsl::string_view ini_string =
    "[Test Header 1]\n"
    "test_int=10\n"
    "test_float=10.0\n"
    "test_string=some string\n"
    "test_bool1=true\n"
    "test_bool2=1\n"
    "\n"
    "[Test Header 2]\n"
    "test_int=20\n"
    "test_float=20.0\n"
    "test_string=some other string\n"
    "test_bool1=false\n"
    "test_bool2=0\n"
    ;

  rex::ini::Ini ini_content = rex::ini::parse(ini_string);

  REX_CHECK(ini_content.is_discarded() == false);
  REX_CHECK(ini_content.all_blocks().size() == 2);

  REX_CHECK(ini_content.get("Test Header 1", "test_int") == "10");
  REX_CHECK(ini_content.get("Test Header 1", "test_float") == "10.0");
  REX_CHECK(ini_content.get("Test Header 1", "test_string") == "some string");
  REX_CHECK(ini_content.get("Test Header 1", "test_bool1") == "true");
  REX_CHECK(ini_content.get("Test Header 1", "test_bool2") == "1");

  REX_CHECK(ini_content.get("Test Header 2", "test_int") == "20");
  REX_CHECK(ini_content.get("Test Header 2", "test_float") == "20.0");
  REX_CHECK(ini_content.get("Test Header 2", "test_string") == "some other string");
  REX_CHECK(ini_content.get("Test Header 2", "test_bool1") == "false");
  REX_CHECK(ini_content.get("Test Header 2", "test_bool2") == "0");
}

TEST_CASE("TEST - Ini - no headers")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rsl::string_view ini_string =
    "test_int=10\n"
    "test_float=10.0\n"
    "test_string=some string\n"
    "test_bool1=true\n"
    "test_bool2=1\n"
    ;

  rex::ini::Ini ini_content = rex::ini::parse(ini_string);

  REX_CHECK(ini_content.is_discarded() == false);
  REX_CHECK(ini_content.all_blocks().size() == 1);

  REX_CHECK(ini_content.get("", "test_int") == "10");
  REX_CHECK(ini_content.get("", "test_float") == "10.0");
  REX_CHECK(ini_content.get("", "test_string") == "some string");
  REX_CHECK(ini_content.get("", "test_bool1") == "true");
  REX_CHECK(ini_content.get("", "test_bool2") == "1");
}

TEST_CASE("TEST - Ini - file without headers")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rex::ini::Ini ini_content = rex::ini::read_from_file("test_settings.ini");

  REX_CHECK(ini_content.is_discarded() == false);
  REX_CHECK(ini_content.all_blocks().size() == 1);

  REX_CHECK(ini_content.get("", "test_int") == "10");
  REX_CHECK(ini_content.get("", "test_float") == "10.0");
  REX_CHECK(ini_content.get("", "test_string") == "some string");
  REX_CHECK(ini_content.get("", "test_bool1") == "true");
  REX_CHECK(ini_content.get("", "test_bool2") == "1");
}

TEST_CASE("TEST - Ini - file with headers")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rex::ini::Ini ini_content = rex::ini::read_from_file("test_settings_with_headers.ini");

  REX_CHECK(ini_content.is_discarded() == false);
  REX_CHECK(ini_content.all_blocks().size() == 2);

  REX_CHECK(ini_content.get("Test Header 1", "test_int") == "10");
  REX_CHECK(ini_content.get("Test Header 1", "test_float") == "10.0");
  REX_CHECK(ini_content.get("Test Header 1", "test_string") == "some string");
  REX_CHECK(ini_content.get("Test Header 1", "test_bool1") == "true");
  REX_CHECK(ini_content.get("Test Header 1", "test_bool2") == "1");

  REX_CHECK(ini_content.get("Test Header 2", "test_int") == "20");
  REX_CHECK(ini_content.get("Test Header 2", "test_float") == "20.0");
  REX_CHECK(ini_content.get("Test Header 2", "test_string") == "some other string");
  REX_CHECK(ini_content.get("Test Header 2", "test_bool1") == "false");
  REX_CHECK(ini_content.get("Test Header 2", "test_bool2") == "0");
}

TEST_CASE("TEST - Ini - error detection")
{
  rex::TempCwd tmp_cwd("settings_tests");

  {
    rsl::string_view ini_string =
      "[Header"
      ;

    rex::ini::Ini ini_content = rex::ini::parse(ini_string);

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rsl::string_view ini_string =
      "[Header]\n"
      "invalid_int="
      ;

    rex::ini::Ini ini_content = rex::ini::parse(ini_string);

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rsl::string_view ini_string =
      "[Header]\n"
      "=10"
      ;

    rex::ini::Ini ini_content = rex::ini::parse(ini_string);

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rsl::string_view ini_string =
      "[Header]\n"
      "="
      ;

    rex::ini::Ini ini_content = rex::ini::parse(ini_string);

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rsl::string_view ini_string =
      "Header]"
      ;

    rex::ini::Ini ini_content = rex::ini::parse(ini_string);

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }
}

TEST_CASE("TEST - Ini - file error detection")
{
  rex::TempCwd tmp_cwd("settings_tests");

  {
    rex::ini::Ini ini_content = rex::ini::read_from_file("invalid_settings_1.ini");

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rex::ini::Ini ini_content = rex::ini::read_from_file("invalid_settings_2.ini");

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rex::ini::Ini ini_content = rex::ini::read_from_file("invalid_settings_3.ini");

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rex::ini::Ini ini_content = rex::ini::read_from_file("invalid_settings_4.ini");

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rex::ini::Ini ini_content = rex::ini::read_from_file("invalid_settings_5.ini");

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }

  {
    rex::ini::Ini ini_content = rex::ini::read_from_file("non_existing_file.ini");

    REX_CHECK(ini_content.is_discarded() == true);
    REX_CHECK(ini_content.all_blocks().empty() == true);
  }
}