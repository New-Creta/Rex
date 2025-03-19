#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/diagnostics/assert.h"

void func_with_clear_name()
{
	rsl::string_view assert_msg = "clear message";
	rex::AssertContext context(assert_msg, rsl::source_location::current());

	REX_CHECK(context.msg() == assert_msg);
	REX_CHECK(context.source_location().file_name().ends_with("test_assert.cpp"));
	REX_CHECK(context.source_location().function_name() == "func_with_clear_name");
	REX_CHECK(context.source_location().line() == 8);
}

TEST_CASE("TEST - Assert - Create assert context in a function with a clear name")
{
	func_with_clear_name();
}
