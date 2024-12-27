#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/string/stringpool.h"
#include "rex_engine/string/stringid.h"

TEST_CASE("TEST - StringPool - Find Or Store")
{
	rsl::string_view string1 = "something";
	rsl::string_view string2 = "else";

	rex::StringID string_id1 = rex::string_pool::find_or_store(string1);
	rex::StringID string_id11 = rex::string_pool::find_or_store(string1);
	rex::StringID string_id2 = rex::string_pool::find_or_store(string2);

	REX_CHECK(string_id1.is_valid());
	REX_CHECK(string_id1.length() == string1.length());
	REX_CHECK(string_id1.string() == string1);
	REX_CHECK(string_id1 == string_id11);

	REX_CHECK(string_id2.is_valid());
	REX_CHECK(string_id2.length() == string2.length());
	REX_CHECK(string_id2.string() == string2);
}