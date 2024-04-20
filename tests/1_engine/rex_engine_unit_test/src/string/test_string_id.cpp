#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/string/stringid.h"
#include "rex_engine/string/stringpool.h"
#include "rex_unit_test/test_string.h"

TEST_CASE("StringID - Invalid String ID")
{
  {  
    rex::StringID sid = rex::StringID::create_invalid();

    REX_CHECK(sid.is_valid() == false);
    REX_CHECK(sid.operator bool() == false);
  }

  {
    rex::StringID sid;

    REX_CHECK(sid.is_valid() == false);
    REX_CHECK(sid.operator bool() == false);
  }
}

TEST_CASE("StringID - Creating of string id")
{
  {
    rsl::string_view string = "something";
    rex::StringID sid = rex::StringID::create(string);

    REX_CHECK(sid.is_valid() == true);
    REX_CHECK(sid.operator bool() == true);
    REX_CHECK(sid.string() == string);
    REX_CHECK(sid.length() == string.length());
  }

  {
    rsl::string_view string = "something";
    rex::StringID sid = "something"_sid;

    REX_CHECK(sid.is_valid() == true);
    REX_CHECK(sid.operator bool() == true);
    REX_CHECK(sid.string() == string);
    REX_CHECK(sid.length() == string.length());
  }
}

TEST_CASE("StringID - equal comparison")
{
  rsl::string_view string = "something";
  rex::StringID sid = rex::StringID::create(string);
  rex::StringID sid2 = sid;

  REX_CHECK(sid.is_valid() == true);
  REX_CHECK(sid.string() == string);
  REX_CHECK(sid.length() == string.length());
  REX_CHECK(sid2.is_valid() == true);
  REX_CHECK(sid2.string() == string);
  REX_CHECK(sid2.length() == string.length());
  REX_CHECK(sid.value() == sid2.value());
  REX_CHECK(sid.operator u32() == sid2.operator u32());
}

TEST_CASE("StringID - not equal comparison")
{
  rsl::string_view string = "something";
  rsl::string_view string2 = "else";
  rex::StringID sid = rex::StringID::create(string);
  rex::StringID sid2 = rex::StringID::create(string2);

  REX_CHECK(sid.is_valid() == true);
  REX_CHECK(sid.string() == string);
  REX_CHECK(sid.length() == string.length());
  REX_CHECK(sid2.is_valid() == true);
  REX_CHECK(sid2.string() == string2);
  REX_CHECK(sid2.length() == string2.length());
  REX_CHECK(sid.value() != sid2.value());
  REX_CHECK(sid.operator u32() != sid2.operator u32());
}

TEST_CASE("StringId - hash collision")
{
  rsl::string_view string = "something";
  rex::StringID sid = rex::StringID::create(string);
  rsl::string string2;

  for (s32 i = 0; i < 1000; ++i)
  {
    string2.clear();
    do
    {
      rex::test::create_random_string(string2, (std::rand() % 10) + 1);
    } while (string == string2);
    
    rex::StringID sid2 = rex::StringID::create(string2);
    REX_CHECK(sid2.is_valid() == true);
    REX_CHECK(sid2.string() == string2);
    REX_CHECK(sid2.length() == string2.length());
    REX_CHECK(sid.value() != sid2.value());
    REX_CHECK(sid.operator u32() != sid2.operator u32());
  }
}