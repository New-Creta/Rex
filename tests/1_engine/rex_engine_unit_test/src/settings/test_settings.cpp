#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/settings/settings.h"


TEST_CASE("Test Settings without headers - with casing")
{
  rex::settings::load("test_settings.ini");

  REX_CHECK(rex::settings::get_int("Test_Int") == 10);
  REX_CHECK(rex::settings::has_setting("Test_Int") == true);
  REX_CHECK(rex::settings::get_float("Test_Float") == 10.0f);
  REX_CHECK(rex::settings::has_setting("Test_Float") == true);
  REX_CHECK(rex::settings::get_string("Test_String") == "some string");
  REX_CHECK(rex::settings::has_setting("Test_String") == true);

  REX_CHECK(rex::settings::get_int("Not_Existing_Int") == 0);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Int") == false);
  REX_CHECK(rex::settings::get_float("Not_Existing_Float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Float") == false);
  REX_CHECK(rex::settings::get_string("Not_Existing_String") == "");
  REX_CHECK(rex::settings::has_setting("Not_Existing_String") == false);

  REX_CHECK(rex::settings::get_int("Not_Existing_Int") == 0);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Int") == false);
  REX_CHECK(rex::settings::get_float("Not_Existing_Float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Float") == false);
  REX_CHECK(rex::settings::get_string("Not_Existing_String") == "");
  REX_CHECK(rex::settings::has_setting("Not_Existing_String") == false);

  REX_CHECK(rex::settings::get_int("Not_Existing_Int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Int") == false);
  REX_CHECK(rex::settings::get_float("Not_Existing_Float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Float") == false);
  REX_CHECK(rex::settings::get_string("Not_Existing_String", "default") == "default");
  REX_CHECK(rex::settings::has_setting("Not_Existing_String") == false);

  rex::settings::unload();
}
TEST_CASE("Test Settings without headers - without casing")
{
  rex::settings::load("test_settings.ini");

  REX_CHECK(rex::settings::get_int("test_int") == 10);
  REX_CHECK(rex::settings::has_setting("test_int") == true);
  REX_CHECK(rex::settings::get_float("test_float") == 10.0f);
  REX_CHECK(rex::settings::has_setting("test_float") == true);
  REX_CHECK(rex::settings::get_string("test_string") == "some string");
  REX_CHECK(rex::settings::has_setting("test_string") == true);

  REX_CHECK(rex::settings::get_int("not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("not_existing_string") == false);

  rex::settings::unload();
}

TEST_CASE("Test Settings with headers - with casing")
{
  rex::settings::load("test_settings_with_headers.ini");

  // First header
  REX_CHECK(rex::settings::get_int("TestHeader1.test_int") == 10);
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_int") == true);
  REX_CHECK(rex::settings::get_float("TestHeader1.test_float") == 10.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_float") == true);
  REX_CHECK(rex::settings::get_string("TestHeader1.test_string") == "some string");
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_string") == true);

  REX_CHECK(rex::settings::get_int("TestHeader1.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader1.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader1.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("TestHeader1.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader1.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader1.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("TestHeader1.not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader1.not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader1.not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_string") == false);

  // Second header
  REX_CHECK(rex::settings::get_int("TestHeader2.test_int") == 20);
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_int") == true);
  REX_CHECK(rex::settings::get_float("TestHeader2.test_float") == 20.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_float") == true);
  REX_CHECK(rex::settings::get_string("TestHeader2.test_string") == "some other string");
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_string") == true);

  REX_CHECK(rex::settings::get_int("TestHeader2.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader2.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader2.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("TestHeader2.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader2.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader2.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("TestHeader2.not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader2.not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader2.not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_string") == false);

  rex::settings::unload();
}

TEST_CASE("Test Settings with headers - without casing")
{
  rex::settings::load("test_settings_with_headers.ini");

  // First header
  REX_CHECK(rex::settings::get_int("testheader1.test_int") == 10);
  REX_CHECK(rex::settings::has_setting("testheader1.test_int") == true);
  REX_CHECK(rex::settings::get_float("testheader1.test_float") == 10.0f);
  REX_CHECK(rex::settings::has_setting("testheader1.test_float") == true);
  REX_CHECK(rex::settings::get_string("testheader1.test_string") == "some string");
  REX_CHECK(rex::settings::has_setting("testheader1.test_string") == true);

  REX_CHECK(rex::settings::get_int("testheader1.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader1.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader1.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("testheader1.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader1.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader1.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("testheader1.not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader1.not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader1.not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_string") == false);

  // Second header
  REX_CHECK(rex::settings::get_int("testheader2.test_int") == 20);
  REX_CHECK(rex::settings::has_setting("testheader2.test_int") == true);
  REX_CHECK(rex::settings::get_float("testheader2.test_float") == 20.0f);
  REX_CHECK(rex::settings::has_setting("testheader2.test_float") == true);
  REX_CHECK(rex::settings::get_string("testheader2.test_string") == "some other string");
  REX_CHECK(rex::settings::has_setting("testheader2.test_string") == true);

  REX_CHECK(rex::settings::get_int("testheader2.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader2.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader2.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("testheader2.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader2.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader2.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_string") == false);

  REX_CHECK(rex::settings::get_int("testheader2.not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader2.not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader2.not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_string") == false);

  rex::settings::unload();
}