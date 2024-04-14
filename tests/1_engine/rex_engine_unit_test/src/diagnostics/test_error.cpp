#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/diagnostics/error.h"

TEST_CASE("Error without error")
{
  rex::Error err = rex::Error::no_error();

  REX_CHECK(err.error_code() == 0);
  REX_CHECK(err.has_error() == false);
  REX_CHECK(err.operator bool() == false);
  REX_CHECK(err.error_msg() == "");
}

TEST_CASE("Error with error")
{
  rex::Error err = rex::Error("some error msg");

  REX_CHECK(err.error_code() != 0);
  REX_CHECK(err.has_error() == true);
  REX_CHECK(err.operator bool() == true);
  REX_CHECK(err.error_msg() == "some error msg");
}