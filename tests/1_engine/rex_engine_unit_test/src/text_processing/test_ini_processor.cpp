#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/text_processing/ini_processor.h"

#include "rex_engine/filesystem/file.h"

TEST_CASE("Ini Processor - no headers")
{
  const rex::memory::Blob content_blob = rex::file::read_file("test_settings.ini");
  rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
  rex::Error error = ini_processor.process();

  REX_CHECK(error.has_error() == false);
  REX_CHECK(ini_processor.items().size() == 1);

  const auto& header_with_items = ini_processor.items().front();
  REX_CHECK(header_with_items.header() == "");
  REX_CHECK(header_with_items.items()[0].key == "test_int");
  REX_CHECK(header_with_items.items()[0].value == "10");
  REX_CHECK(header_with_items.items()[1].key == "test_float");
  REX_CHECK(header_with_items.items()[1].value == "10.0");
  REX_CHECK(header_with_items.items()[2].key == "test_string");
  REX_CHECK(header_with_items.items()[2].value == "some string");
}

TEST_CASE("Ini Processor - with headers")
{
  const rex::memory::Blob content_blob = rex::file::read_file("test_settings_with_headers.ini");
  rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
  rex::Error error = ini_processor.process();

  REX_CHECK(error.has_error() == false);
  REX_CHECK(ini_processor.items().size() == 2);

  auto header_with_items = ini_processor.items()[0];
  REX_CHECK(header_with_items.header() == "Test Header 1");
  REX_CHECK(header_with_items.items()[0].key == "test_int");
  REX_CHECK(header_with_items.items()[0].value == "10");
  REX_CHECK(header_with_items.items()[1].key == "test_float");
  REX_CHECK(header_with_items.items()[1].value == "10.0");
  REX_CHECK(header_with_items.items()[2].key == "test_string");
  REX_CHECK(header_with_items.items()[2].value == "some string");

  header_with_items = ini_processor.items()[1];
  REX_CHECK(header_with_items.header() == "Test Header 2");
  REX_CHECK(header_with_items.items()[0].key == "test_int");
  REX_CHECK(header_with_items.items()[0].value == "20");
  REX_CHECK(header_with_items.items()[1].key == "test_float");
  REX_CHECK(header_with_items.items()[1].value == "20.0");
  REX_CHECK(header_with_items.items()[2].key == "test_string");
  REX_CHECK(header_with_items.items()[2].value == "some other string");
}

TEST_CASE("Ini Processor - error detection")
{
  {
    const rex::memory::Blob content_blob = rex::file::read_file("invalid_settings_1.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
    rex::Error error = ini_processor.process();

    REX_CHECK(error.has_error() == true);
    REX_CHECK(ini_processor.items().empty() == true);
  }

  {
    const rex::memory::Blob content_blob = rex::file::read_file("invalid_settings_2.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
    rex::Error error = ini_processor.process();

    REX_CHECK(error.has_error() == true);
    REX_CHECK(ini_processor.items().empty() == true);
  }

  {
    const rex::memory::Blob content_blob = rex::file::read_file("invalid_settings_3.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
    rex::Error error = ini_processor.process();

    REX_CHECK(error.has_error() == true);
    REX_CHECK(ini_processor.items().empty() == true);
  }

  {
    const rex::memory::Blob content_blob = rex::file::read_file("invalid_settings_4.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(rex::memory::BlobView(content_blob));
    rex::Error error = ini_processor.process();

    REX_CHECK(error.has_error() == true);
    REX_CHECK(ini_processor.items().empty() == true);
  }
}