#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/diagnostics/stacktrace.h"

void function_with_clear_name1()
{
	rex::ResolvedCallstack callstack(rex::current_callstack());
	REX_CHECK(callstack[0].source_file().ends_with("test_stacktrace.cpp"));
	REX_CHECK(callstack[0].description().contains("function_with_clear_name1"));
	REX_CHECK(callstack[0].source_line() == 7);

	REX_CHECK(callstack[1].source_file().ends_with("test_stacktrace.cpp"));
	REX_CHECK(callstack[1].description().contains("function_with_clear_name2"));
	REX_CHECK(callstack[1].source_line() == 20);

}
void function_with_clear_name2()
{
	function_with_clear_name1();
}

TEST_CASE("TEST - Callstack - Resolution")
{
	function_with_clear_name2();
}