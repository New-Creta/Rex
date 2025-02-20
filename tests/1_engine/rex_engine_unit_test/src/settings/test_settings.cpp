#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/settings/settings.h"
#include "rex_engine/filesystem/tmp_cwd.h"

TEST_CASE("TEST - Settings - Settings without headers - with casing")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rex::settings::load("test_settings.ini");

  REX_CHECK(rex::settings::get_int("Test_Int") == 10);
  REX_CHECK(rex::settings::has_setting("Test_Int") == true);
  REX_CHECK(rex::settings::get_float("Test_Float") == 10.0f);
  REX_CHECK(rex::settings::has_setting("Test_Float") == true);
  REX_CHECK(rex::settings::get_string("Test_String") == "some string");
  REX_CHECK(rex::settings::has_setting("Test_String") == true);
  REX_CHECK(rex::settings::get_bool("Test_Bool1") == true);
  REX_CHECK(rex::settings::has_setting("Test_Bool1") == true);
  REX_CHECK(rex::settings::get_bool("Test_Bool2") == true);
  REX_CHECK(rex::settings::has_setting("Test_Bool2") == true);

  REX_CHECK(rex::settings::get_int("Not_Existing_Int") == 0);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Int") == false);
  REX_CHECK(rex::settings::get_float("Not_Existing_Float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Float") == false);
  REX_CHECK(rex::settings::get_string("Not_Existing_String") == "");
  REX_CHECK(rex::settings::has_setting("Not_Existing_String") == false);
  REX_CHECK(rex::settings::get_bool("Not_Existing_Bool") == false);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Bool") == false);

  REX_CHECK(rex::settings::get_int("Not_Existing_Int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Int") == false);
  REX_CHECK(rex::settings::get_float("Not_Existing_Float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Float") == false);
  REX_CHECK(rex::settings::get_string("Not_Existing_String", "default") == "default");
  REX_CHECK(rex::settings::has_setting("Not_Existing_String") == false);
  REX_CHECK(rex::settings::get_bool("Not_Existing_Bool", true) == true);
  REX_CHECK(rex::settings::has_setting("Not_Existing_Bool") == false);

  rex::settings::unload();
}
TEST_CASE("TEST - Settings - Settings without headers - without casing")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rex::settings::load("test_settings.ini");

  REX_CHECK(rex::settings::get_int("test_int") == 10);
  REX_CHECK(rex::settings::has_setting("test_int") == true);
  REX_CHECK(rex::settings::get_float("test_float") == 10.0f);
  REX_CHECK(rex::settings::has_setting("test_float") == true);
  REX_CHECK(rex::settings::get_string("test_string") == "some string");
  REX_CHECK(rex::settings::has_setting("test_string") == true);
  REX_CHECK(rex::settings::get_bool("test_bool1") == true);
  REX_CHECK(rex::settings::has_setting("test_bool1") == true);
  REX_CHECK(rex::settings::get_bool("test_bool2") == true);
  REX_CHECK(rex::settings::has_setting("test_bool2") == true);

  REX_CHECK(rex::settings::get_int("not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("not_existing_string") == false);
  REX_CHECK(rex::settings::get_bool("not_existing_bool") == false);
  REX_CHECK(rex::settings::has_setting("not_existing_bool") == false);

  REX_CHECK(rex::settings::get_int("not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("not_existing_string") == false);
  REX_CHECK(rex::settings::get_bool("not_existing_bool", true) == true);
  REX_CHECK(rex::settings::has_setting("not_existing_bool") == false);

  rex::settings::unload();
}
TEST_CASE("TEST - Settings - Settings with headers - with casing")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rex::settings::load("test_settings_with_headers.ini");

  // First header
  REX_CHECK(rex::settings::get_int("TestHeader1.test_int") == 10);
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_int") == true);
  REX_CHECK(rex::settings::get_float("TestHeader1.test_float") == 10.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_float") == true);
  REX_CHECK(rex::settings::get_string("TestHeader1.test_string") == "some string");
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_string") == true);
  REX_CHECK(rex::settings::get_bool("TestHeader1.test_bool1") == true);
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_bool1") == true);
  REX_CHECK(rex::settings::get_bool("TestHeader1.test_bool2") == true);
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_bool1") == true);

  REX_CHECK(rex::settings::get_int("TestHeader1.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader1.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader1.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_string") == false);
  REX_CHECK(rex::settings::get_int("TestHeader1.not_existing_bool") == 0);
  REX_CHECK(rex::settings::get_bool("TestHeader1.not_existing_bool") == false);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_bool") == false);

  REX_CHECK(rex::settings::get_int("TestHeader1.not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader1.not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader1.not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("TestHeader1.not_existing_string") == false);
  REX_CHECK(rex::settings::get_int("TestHeader1.test_bool", 1) == 1);
  REX_CHECK(rex::settings::get_bool("TestHeader1.test_bool", true) == true);
  REX_CHECK(rex::settings::has_setting("TestHeader1.test_bool") == false);

  // Second header
  REX_CHECK(rex::settings::get_int("TestHeader2.test_int") == 20);
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_int") == true);
  REX_CHECK(rex::settings::get_float("TestHeader2.test_float") == 20.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_float") == true);
  REX_CHECK(rex::settings::get_string("TestHeader2.test_string") == "some other string");
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_string") == true);
  REX_CHECK(rex::settings::get_bool("TestHeader2.test_bool1") == false);
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_bool1") == true);
  REX_CHECK(rex::settings::get_bool("TestHeader2.test_bool2") == false);
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_bool1") == true);

  REX_CHECK(rex::settings::get_int("TestHeader2.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader2.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader2.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_string") == false);
  REX_CHECK(rex::settings::get_int("TestHeader2.not_existing_bool") == 0);
  REX_CHECK(rex::settings::get_bool("TestHeader2.not_existing_bool") == false);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_bool") == false);

  REX_CHECK(rex::settings::get_int("TestHeader2.not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("TestHeader2.not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("TestHeader2.not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("TestHeader2.not_existing_string") == false);
  REX_CHECK(rex::settings::get_int("TestHeader2.test_bool", true) == 1);
  REX_CHECK(rex::settings::get_bool("TestHeader2.test_bool", true) == true);
  REX_CHECK(rex::settings::has_setting("TestHeader2.test_bool") == false);

  rex::settings::unload();
}
TEST_CASE("TEST - Settings - Settings with headers - without casing")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rex::settings::load("test_settings_with_headers.ini");

  // First header
  REX_CHECK(rex::settings::get_int("testheader1.test_int") == 10);
  REX_CHECK(rex::settings::has_setting("testheader1.test_int") == true);
  REX_CHECK(rex::settings::get_float("testheader1.test_float") == 10.0f);
  REX_CHECK(rex::settings::has_setting("testheader1.test_float") == true);
  REX_CHECK(rex::settings::get_string("testheader1.test_string") == "some string");
  REX_CHECK(rex::settings::has_setting("testheader1.test_string") == true);
  REX_CHECK(rex::settings::get_bool("testheader1.test_bool1") == true);
  REX_CHECK(rex::settings::has_setting("testheader1.test_bool1") == true);
  REX_CHECK(rex::settings::get_bool("testheader1.test_bool2") == true);
  REX_CHECK(rex::settings::has_setting("testheader1.test_bool1") == true);

  REX_CHECK(rex::settings::get_int("testheader1.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader1.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader1.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_string") == false);
  REX_CHECK(rex::settings::get_int("testheader1.not_existing_bool") == 0);
  REX_CHECK(rex::settings::get_bool("testheader1.not_existing_bool") == false);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_bool") == false);

  REX_CHECK(rex::settings::get_int("testheader1.not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader1.not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader1.not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_string") == false);
  REX_CHECK(rex::settings::get_bool("testheader1.test_bool", true) == true);
  REX_CHECK(rex::settings::get_int("testheader1.test_bool", true) == 1);
  REX_CHECK(rex::settings::has_setting("testheader1.test_bool") == false);

  // Second header
  REX_CHECK(rex::settings::get_int("testheader2.test_int") == 20);
  REX_CHECK(rex::settings::has_setting("testheader2.test_int") == true);
  REX_CHECK(rex::settings::get_float("testheader2.test_float") == 20.0f);
  REX_CHECK(rex::settings::has_setting("testheader2.test_float") == true);
  REX_CHECK(rex::settings::get_string("testheader2.test_string") == "some other string");
  REX_CHECK(rex::settings::has_setting("testheader2.test_string") == true);
  REX_CHECK(rex::settings::get_bool("testheader2.test_bool1") == false);
  REX_CHECK(rex::settings::has_setting("testheader2.test_bool1") == true);
  REX_CHECK(rex::settings::get_bool("testheader2.test_bool2") == false);
  REX_CHECK(rex::settings::has_setting("testheader2.test_bool1") == true);

  REX_CHECK(rex::settings::get_int("testheader2.not_existing_int") == 0);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader2.not_existing_float") == 0.0f);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader2.not_existing_string") == "");
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_string") == false);
  REX_CHECK(rex::settings::get_int("testheader1.not_existing_bool") == 0);
  REX_CHECK(rex::settings::get_bool("testheader1.not_existing_bool") == false);
  REX_CHECK(rex::settings::has_setting("testheader1.not_existing_bool") == false);

  REX_CHECK(rex::settings::get_int("testheader2.not_existing_int", 5) == 5);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_int") == false);
  REX_CHECK(rex::settings::get_float("testheader2.not_existing_float", 5.0f) == 5.0f);
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_float") == false);
  REX_CHECK(rex::settings::get_string("testheader2.not_existing_string", "default") == "default");
  REX_CHECK(rex::settings::has_setting("testheader2.not_existing_string") == false);
  REX_CHECK(rex::settings::get_int("testheader1.test_bool", true) == 1);
  REX_CHECK(rex::settings::get_bool("testheader1.test_bool", true) == true);
  REX_CHECK(rex::settings::has_setting("testheader1.test_bool") == false);

  rex::settings::unload();
}
TEST_CASE("TEST - Settings - Setting a setting")
{
  rex::TempCwd tmp_cwd("settings_tests");

  rex::settings::set("New_Int1", 20);
  rex::settings::set("New_Float1", 30.0f);
  rex::settings::set("New_String1", "default");
  rex::settings::set("New_Bool1", true);

  REX_CHECK(rex::settings::has_setting("New_Int1") == true);
  REX_CHECK(rex::settings::has_setting("New_Float1") == true);
  REX_CHECK(rex::settings::has_setting("New_String1") == true);
  REX_CHECK(rex::settings::has_setting("New_Bool1") == true);

  REX_CHECK(rex::settings::get_int("New_Int1") == 20);
  REX_CHECK(rex::settings::get_float("New_Float1") == 30.0f);
  REX_CHECK(rex::settings::get_string("New_String1") == "default");
  REX_CHECK(rex::settings::get_bool("New_Bool1") == true);

  rex::settings::set("new_int2", 20);
  rex::settings::set("new_float2", 30.0f);
  rex::settings::set("new_string2", "default");
  rex::settings::set("new_bool2", true);

  REX_CHECK(rex::settings::has_setting("new_int2") == true);
  REX_CHECK(rex::settings::has_setting("new_float2") == true);
  REX_CHECK(rex::settings::has_setting("new_string2") == true);
  REX_CHECK(rex::settings::has_setting("new_bool2") == true);

  REX_CHECK(rex::settings::get_int("new_int2") == 20);
  REX_CHECK(rex::settings::get_float("new_float2") == 30.0f);
  REX_CHECK(rex::settings::get_string("new_string2") == "default");
  REX_CHECK(rex::settings::get_bool("new_bool2") == true);

  rex::settings::load("test_settings.ini");

  REX_CHECK(rex::settings::get_int("Test_Int") == 10);
  REX_CHECK(rex::settings::get_float("Test_Float") == 10.0f);
  REX_CHECK(rex::settings::get_string("Test_String") == "some string");
  REX_CHECK(rex::settings::get_bool("Test_Bool1") == true);
  REX_CHECK(rex::settings::get_bool("Test_Bool2") == true);

  rex::settings::set("test_int", 70);
  rex::settings::set("test_float", 50.0f);
  rex::settings::set("test_string", "default2");
  rex::settings::set("test_bool1", false);
  rex::settings::set("test_bool2", false);

  REX_CHECK(rex::settings::get_int("Test_Int") == 70);
  REX_CHECK(rex::settings::get_float("Test_Float") == 50.0f);
  REX_CHECK(rex::settings::get_string("Test_String") == "default2");
  REX_CHECK(rex::settings::get_bool("Test_Bool1") == false);
  REX_CHECK(rex::settings::get_bool("Test_Bool2") == false);

  rex::settings::unload();
}