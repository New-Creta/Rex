#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/text_processing/ini_processor.h"

#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/tmp_cwd.h"

TEST_CASE("TEST - Ini Processor - no headers")
{
  rex::TempCwd tmp_cwd("settings_tests");

  const rex::memory::Blob content_blob = rex::file::read_file("test_settings.ini");
  rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
  rex::Error error = ini_processor.process();

  REX_CHECK(error.has_error() == false);
  REX_CHECK(ini_processor.all_items().size() == 1);

  const auto& header_with_items = *ini_processor.all_items().cbegin();
  REX_CHECK(header_with_items.value.header() == "");
  REX_CHECK(header_with_items.value.get("test_int") == "10");
  REX_CHECK(header_with_items.value.get("test_float") == "10.0");
  REX_CHECK(header_with_items.value.get("test_string") == "some string");
}

TEST_CASE("TEST - Ini Processor - with headers")
{
  rex::TempCwd tmp_cwd("settings_tests");

  const rex::memory::Blob content_blob = rex::file::read_file("test_settings_with_headers.ini");
  rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
  rex::Error error = ini_processor.process();

  REX_CHECK(error.has_error() == false);
  REX_CHECK(ini_processor.all_items().size() == 2);

  REX_CHECK(ini_processor.get("Test Header 1", "test_int") == "10");
  REX_CHECK(ini_processor.get("Test Header 1", "test_float") == "10.0");
  REX_CHECK(ini_processor.get("Test Header 1", "test_string") == "some string");

  REX_CHECK(ini_processor.get("Test Header 2", "test_int") == "20");
  REX_CHECK(ini_processor.get("Test Header 2", "test_float") == "20.0");
  REX_CHECK(ini_processor.get("Test Header 2", "test_string") == "some other string");
}

TEST_CASE("TEST - Ini Processor - error detection")
{
  rex::TempCwd tmp_cwd("settings_tests");

  {
    const rex::memory::Blob content_blob = rex::file::read_file("invalid_settings_1.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
    rex::Error error = ini_processor.process();

    REX_CHECK(error.has_error() == true);
    REX_CHECK(ini_processor.all_items().empty() == true);
  }

  {
    const rex::memory::Blob content_blob = rex::file::read_file("invalid_settings_2.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
    rex::Error error = ini_processor.process();

    REX_CHECK(error.has_error() == true);
    REX_CHECK(ini_processor.all_items().empty() == true);
  }

  {
    const rex::memory::Blob content_blob = rex::file::read_file("invalid_settings_3.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
    rex::Error error = ini_processor.process();

    REX_CHECK(error.has_error() == true);
    REX_CHECK(ini_processor.all_items().empty() == true);
  }

  {
    const rex::memory::Blob content_blob = rex::file::read_file("invalid_settings_4.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
    rex::Error error = ini_processor.process();

    REX_CHECK(error.has_error() == true);
    REX_CHECK(ini_processor.all_items().empty() == true);
  }
}