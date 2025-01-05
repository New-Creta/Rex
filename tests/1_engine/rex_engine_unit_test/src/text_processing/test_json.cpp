#include "rex_unit_test/rex_catch2.h"

#define JSON_TESTS_PRIVATE

#include "rex_engine/text_processing/json.h"

namespace
{
	class SaxEventLogger
	{
	public:
		bool null()
		{
			events.emplace_back("null()");
			return true;
		}

		bool boolean(bool val)
		{
			events.emplace_back(val ? "boolean(true)" : "boolean(false)");
			return true;
		}

		bool number_integer(rex::json::json::number_integer_t val)
		{
			events.push_back("number_integer(" + rsl::to_string(val) + ")");
			return true;
		}

		bool number_unsigned(rex::json::json::number_unsigned_t val)
		{
			events.push_back("number_unsigned(" + rsl::to_string(val) + ")");
			return true;
		}

		bool number_float(rex::json::json::number_float_t /*unused*/, const rsl::string& s)
		{
			events.push_back("number_float(" + s + ")");
			return true;
		}

		bool string(rsl::string& val)
		{
			events.push_back("string(" + val + ")");
			return true;
		}

		bool binary(rex::json::json::binary_t& val)
		{
			rsl::string binary_contents("binary(");
			rsl::string comma_space;
			for (auto b : val)
			{
				binary_contents.append(comma_space);
				binary_contents.append(rsl::to_string(static_cast<int>(b)));
				comma_space.assign(", ");
			}
			binary_contents.append(")");
			events.push_back(binary_contents);
			return true;
		}

		bool start_object(rsl::size_t elements)
		{
			if (elements == static_cast<rsl::size_t>(-1))
			{
				events.emplace_back("start_object()");
			}
			else
			{
				events.push_back("start_object(" + rsl::to_string(elements) + ")");
			}
			return true;
		}

		bool key(rsl::string& val)
		{
			events.push_back("key(" + val + ")");
			return true;
		}

		bool end_object()
		{
			events.emplace_back("end_object()");
			return true;
		}

		bool start_array(rsl::size_t elements)
		{
			if (elements == static_cast<rsl::size_t>(-1))
			{
				events.emplace_back("start_array()");
			}
			else
			{
				events.push_back("start_array(" + rsl::to_string(elements) + ")");
			}
			return true;
		}

		bool end_array()
		{
			events.emplace_back("end_array()");
			return true;
		}

		bool parse_error(rsl::size_t position, rsl::string_view /*unused*/, const rex::json::json::exception& /*unused*/)
		{
			errored = true;
			events.push_back("parse_error(" + rsl::to_string(position) + ")");
			return false;
		}

		rsl::vector<rsl::string> events{};
		bool errored = false;
	};

	class SaxCountdown : public rex::json::json::json_sax_t
	{
	public:
		explicit SaxCountdown(const int count) : events_left(count)
		{}

		bool null() override
		{
			return events_left-- > 0;
		}

		bool boolean(bool /*val*/) override
		{
			return events_left-- > 0;
		}

		bool number_integer(rex::json::json::number_integer_t /*val*/) override
		{
			return events_left-- > 0;
		}

		bool number_unsigned(rex::json::json::number_unsigned_t /*val*/) override
		{
			return events_left-- > 0;
		}

		bool number_float(rex::json::json::number_float_t /*val*/, const rsl::string& /*s*/) override
		{
			return events_left-- > 0;
		}

		bool string(rsl::string& /*val*/) override
		{
			return events_left-- > 0;
		}

		bool binary(rex::json::json::binary_t& /*val*/) override
		{
			return events_left-- > 0;
		}

		bool start_object(rsl::size_t /*elements*/) override
		{
			return events_left-- > 0;
		}

		bool key(rsl::string& /*val*/) override
		{
			return events_left-- > 0;
		}

		bool end_object() override
		{
			return events_left-- > 0;
		}

		bool start_array(rsl::size_t /*elements*/) override
		{
			return events_left-- > 0;
		}

		bool end_array() override
		{
			return events_left-- > 0;
		}

		bool parse_error(rsl::size_t /*position*/, const rsl::string& /*last_token*/, const rex::json::json::exception& /*ex*/) override
		{
			return false;
		}

	private:
		int events_left = 0;
	};

	rex::json::json parser_helper(rsl::string_view s);
	bool accept_helper(rsl::string_view s);
	void comments_helper(rsl::string_view s);

	rex::json::json parser_helper(rsl::string_view s)
	{
		rex::json::json j;
		rex::json::json::parser(rex::json::detail::input_adapter(s)).parse(true, j);

		if (j.is_discarded())
		{
			return j;
		}

		rex::json::json j_sax;
		rex::json::detail::json_sax_dom_parser<rex::json::json> sdp(j_sax);
		rex::json::json::sax_parse(s, &sdp);
		REX_CHECK(j_sax == j);

		comments_helper(s);

		return j;
	}

	bool accept_helper(rsl::string_view s)
	{
		CAPTURE(s);

		// 1. parse s without exceptions
		rex::json::json j;
		rex::json::json::parser(nlohmann::detail::input_adapter(s), nullptr, false).parse(true, j);
		const bool ok_noexcept = !j.is_discarded();

		// 2. accept s
		const bool ok_accept = rex::json::json::parser(nlohmann::detail::input_adapter(s)).accept(true);

		// 3. check if both approaches come to the same result
		REX_CHECK(ok_noexcept == ok_accept);

		// 4. parse with SAX (compare with relaxed accept result)
		SaxEventLogger el;
		rex::json::json::sax_parse(s, &el, rex::json::json::input_format_t::json, false);
		REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s)).accept(false) == !el.errored);

		// 5. parse with simple callback
		rex::json::json::parser_callback_t const cb = [](int /*unused*/, rex::json::json::parse_event_t /*unused*/, rex::json::json& /*unused*/) noexcept
		{
			return true;
		};
		rex::json::json const j_cb = rex::json::json::parse(s, cb, false);
		const bool ok_noexcept_cb = !j_cb.is_discarded();

		// 6. check if this approach came to the same result
		REX_CHECK(ok_noexcept == ok_noexcept_cb);

		// 7. check if comments are properly ignored
		if (ok_accept)
		{
			comments_helper(s);
		}

		// 8. return result
		return ok_accept;
	}

	void comments_helper(rsl::string_view s)
	{
		rex::json::json _;

		// parse/accept with default parser
		rex::json::json::parse(s);
		REX_CHECK(rex::json::json::accept(s));

		// parse/accept while skipping comments
		rex::json::json::parse(s, nullptr, false, true);
		REX_CHECK(rex::json::json::accept(s, true));

		rsl::vector<rsl::string> json_with_comments;

		// start with a comment
		json_with_comments.push_back(rsl::string("// this is a comment\n") + s);
		json_with_comments.push_back(rsl::string("/* this is a comment */") + s);
		// end with a comment
		json_with_comments.push_back(rsl::string(s) + "// this is a comment");
		json_with_comments.push_back(rsl::string(s) + "/* this is a comment */");

		// check all strings
		for (const auto& json_with_comment : json_with_comments)
		{
			CAPTURE(json_with_comment);
			REX_CHECK(rex::json::json::parse(json_with_comment).is_discarded() == true);
			REX_CHECK(!rex::json::json::accept(json_with_comment));

			rex::json::json::parse(json_with_comment, nullptr, false, true);
			REX_CHECK(rex::json::json::accept(json_with_comment, true));
		}
	}

} // namespace

TEST_CASE("TEST - Text Processing - Json")
{
	SECTION("SECTION - parse")
	{
		SECTION("SECTION - null")
		{
			REX_CHECK(parser_helper("null") == rex::json::json(nullptr));
		}

		SECTION("SECTION - true")
		{
			REX_CHECK(parser_helper("true") == rex::json::json(true));
		}

		SECTION("SECTION - false")
		{
			REX_CHECK(parser_helper("false") == rex::json::json(false));
		}

		SECTION("SECTION - array")
		{
			SECTION("SECTION - empty array")
			{
				REX_CHECK(parser_helper("[]") == rex::json::json(rex::json::json::value_t::array));
				REX_CHECK(parser_helper("[ ]") == rex::json::json(rex::json::json::value_t::array));
			}

			SECTION("SECTION - nonempty array")
			{
				REX_CHECK(parser_helper("[true, false, null]") == rex::json::json({ true, false, nullptr }));
			}
		}

		SECTION("SECTION - object")
		{
			SECTION("SECTION - empty object")
			{
				REX_CHECK(parser_helper("{}") == rex::json::json(rex::json::json::value_t::object));
				REX_CHECK(parser_helper("{ }") == rex::json::json(rex::json::json::value_t::object));
			}

			SECTION("SECTION - nonempty object")
			{
				REX_CHECK(parser_helper("{\"\": true, \"one\": 1, \"two\": null}") == rex::json::json({ {"", true}, {"one", 1}, {"two", nullptr} }));
			}
		}

		SECTION("SECTION - string")
		{
			// empty string
			REX_CHECK(parser_helper("\"\"") == rex::json::json(rex::json::json::value_t::string));

			SECTION("SECTION - errors")
			{
				// error: tab in string
				REX_CHECK(parser_helper("\"\t\"").is_discarded() == true);
				// error: newline in string
				REX_CHECK(parser_helper("\"\n\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\r\"").is_discarded() == true);
				// error: backspace in string
				REX_CHECK(parser_helper("\"\b\"").is_discarded() == true);
				// improve code coverage
				REX_CHECK(parser_helper("\uFF01").is_discarded() == true);
				REX_CHECK(parser_helper("[-4:1,]").is_discarded() == true);
				// unescaped control characters
				REX_CHECK(parser_helper("\"\x00\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x01\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x02\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x03\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x04\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x05\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x06\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x07\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x08\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x09\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x0a\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x0b\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x0c\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x0d\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x0e\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x0f\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x10\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x11\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x12\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x13\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x14\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x15\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x16\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x17\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x18\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x19\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x1a\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x1b\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x1c\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x1d\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x1e\"").is_discarded() == true);
				REX_CHECK(parser_helper("\"\x1f\"").is_discarded() == true);

				SECTION("SECTION - additional test for null byte")
				{
					// The test above for the null byte is wrong, because passing
					// a string to the parser only reads int until it encounters
					// a null byte. This test inserts the null byte later on and
					// uses an iterator range.
					rsl::string s("\"1\"");
					s[1] = '\0';
					rex::json::json _;
					REX_CHECK(rex::json::json::parse(s.begin(), s.end()).is_discarded() == true);
				}
			}

			SECTION("SECTION - escaped")
			{
				// quotation mark "\""
				auto r1 = R"("\"")"_json;
				REX_CHECK(parser_helper("\"\\\"\"") == r1);
				// reverse solidus "\\"
				auto r2 = R"("\\")"_json;
				REX_CHECK(parser_helper("\"\\\\\"") == r2);
				// solidus
				REX_CHECK(parser_helper("\"\\/\"") == R"("/")"_json);
				// backspace
				REX_CHECK(parser_helper("\"\\b\"") == rex::json::json("\b"));
				// formfeed
				REX_CHECK(parser_helper("\"\\f\"") == rex::json::json("\f"));
				// newline
				REX_CHECK(parser_helper("\"\\n\"") == rex::json::json("\n"));
				// carriage return
				REX_CHECK(parser_helper("\"\\r\"") == rex::json::json("\r"));
				// horizontal tab
				REX_CHECK(parser_helper("\"\\t\"") == rex::json::json("\t"));

				REX_CHECK(parser_helper("\"\\u0001\"").get<rex::json::json::string_t>() == "\x01");
				REX_CHECK(parser_helper("\"\\u000a\"").get<rex::json::json::string_t>() == "\n");
				REX_CHECK(parser_helper("\"\\u00b0\"").get<rex::json::json::string_t>() == "°");
				REX_CHECK(parser_helper("\"\\u0c00\"").get<rex::json::json::string_t>() == "ఀ");
				REX_CHECK(parser_helper("\"\\ud000\"").get<rex::json::json::string_t>() == "퀀");
				REX_CHECK(parser_helper("\"\\u000E\"").get<rex::json::json::string_t>() == "\x0E");
				REX_CHECK(parser_helper("\"\\u00F0\"").get<rex::json::json::string_t>() == "ð");
				REX_CHECK(parser_helper("\"\\u0100\"").get<rex::json::json::string_t>() == "Ā");
				REX_CHECK(parser_helper("\"\\u2000\"").get<rex::json::json::string_t>() == " ");
				REX_CHECK(parser_helper("\"\\uFFFF\"").get<rex::json::json::string_t>() == "￿");
				REX_CHECK(parser_helper("\"\\u20AC\"").get<rex::json::json::string_t>() == "€");
				REX_CHECK(parser_helper("\"€\"").get<rex::json::json::string_t>() == "€");
				REX_CHECK(parser_helper("\"🎈\"").get<rex::json::json::string_t>() == "🎈");

				REX_CHECK(parser_helper("\"\\ud80c\\udc60\"").get<rex::json::json::string_t>() == "\xf0\x93\x81\xa0");
				REX_CHECK(parser_helper("\"\\ud83c\\udf1e\"").get<rex::json::json::string_t>() == "🌞");
			}
		}

		SECTION("SECTION - number")
		{
			SECTION("SECTION - integers")
			{
				SECTION("SECTION - without exponent")
				{
					REX_CHECK(parser_helper("-128") == rex::json::json(-128));
					REX_CHECK(parser_helper("-0") == rex::json::json(-0));
					REX_CHECK(parser_helper("0") == rex::json::json(0));
					REX_CHECK(parser_helper("128") == rex::json::json(128));
				}

				SECTION("SECTION - with exponent")
				{
					REX_CHECK(parser_helper("0e1") == rex::json::json(0e1));
					REX_CHECK(parser_helper("0E1") == rex::json::json(0e1));

					REX_CHECK(parser_helper("10000E-4") == rex::json::json(10000e-4));
					REX_CHECK(parser_helper("10000E-3") == rex::json::json(10000e-3));
					REX_CHECK(parser_helper("10000E-2") == rex::json::json(10000e-2));
					REX_CHECK(parser_helper("10000E-1") == rex::json::json(10000e-1));
					REX_CHECK(parser_helper("10000E0") == rex::json::json(10000e0));
					REX_CHECK(parser_helper("10000E1") == rex::json::json(10000e1));
					REX_CHECK(parser_helper("10000E2") == rex::json::json(10000e2));
					REX_CHECK(parser_helper("10000E3") == rex::json::json(10000e3));
					REX_CHECK(parser_helper("10000E4") == rex::json::json(10000e4));

					REX_CHECK(parser_helper("10000e-4") == rex::json::json(10000e-4));
					REX_CHECK(parser_helper("10000e-3") == rex::json::json(10000e-3));
					REX_CHECK(parser_helper("10000e-2") == rex::json::json(10000e-2));
					REX_CHECK(parser_helper("10000e-1") == rex::json::json(10000e-1));
					REX_CHECK(parser_helper("10000e0") == rex::json::json(10000e0));
					REX_CHECK(parser_helper("10000e1") == rex::json::json(10000e1));
					REX_CHECK(parser_helper("10000e2") == rex::json::json(10000e2));
					REX_CHECK(parser_helper("10000e3") == rex::json::json(10000e3));
					REX_CHECK(parser_helper("10000e4") == rex::json::json(10000e4));

					REX_CHECK(parser_helper("-0e1") == rex::json::json(-0e1));
					REX_CHECK(parser_helper("-0E1") == rex::json::json(-0e1));
					REX_CHECK(parser_helper("-0E123") == rex::json::json(-0e123));

					// numbers after exponent
					REX_CHECK(parser_helper("10E0") == rex::json::json(10e0));
					REX_CHECK(parser_helper("10E1") == rex::json::json(10e1));
					REX_CHECK(parser_helper("10E2") == rex::json::json(10e2));
					REX_CHECK(parser_helper("10E3") == rex::json::json(10e3));
					REX_CHECK(parser_helper("10E4") == rex::json::json(10e4));
					REX_CHECK(parser_helper("10E5") == rex::json::json(10e5));
					REX_CHECK(parser_helper("10E6") == rex::json::json(10e6));
					REX_CHECK(parser_helper("10E7") == rex::json::json(10e7));
					REX_CHECK(parser_helper("10E8") == rex::json::json(10e8));
					REX_CHECK(parser_helper("10E9") == rex::json::json(10e9));
					REX_CHECK(parser_helper("10E+0") == rex::json::json(10e0));
					REX_CHECK(parser_helper("10E+1") == rex::json::json(10e1));
					REX_CHECK(parser_helper("10E+2") == rex::json::json(10e2));
					REX_CHECK(parser_helper("10E+3") == rex::json::json(10e3));
					REX_CHECK(parser_helper("10E+4") == rex::json::json(10e4));
					REX_CHECK(parser_helper("10E+5") == rex::json::json(10e5));
					REX_CHECK(parser_helper("10E+6") == rex::json::json(10e6));
					REX_CHECK(parser_helper("10E+7") == rex::json::json(10e7));
					REX_CHECK(parser_helper("10E+8") == rex::json::json(10e8));
					REX_CHECK(parser_helper("10E+9") == rex::json::json(10e9));
					REX_CHECK(parser_helper("10E-1") == rex::json::json(10e-1));
					REX_CHECK(parser_helper("10E-2") == rex::json::json(10e-2));
					REX_CHECK(parser_helper("10E-3") == rex::json::json(10e-3));
					REX_CHECK(parser_helper("10E-4") == rex::json::json(10e-4));
					REX_CHECK(parser_helper("10E-5") == rex::json::json(10e-5));
					REX_CHECK(parser_helper("10E-6") == rex::json::json(10e-6));
					REX_CHECK(parser_helper("10E-7") == rex::json::json(10e-7));
					REX_CHECK(parser_helper("10E-8") == rex::json::json(10e-8));
					REX_CHECK(parser_helper("10E-9") == rex::json::json(10e-9));
				}

				SECTION("SECTION - edge cases")
				{
					// From RFC8259, Section 6:
					// Note that when such software is used, numbers that are
					// integers and are in the range [-(2**53)+1, (2**53)-1]
					// are interoperable in the sense that implementations will
					// agree exactly on their numeric values.

					// -(2**53)+1
					REX_CHECK(parser_helper("-9007199254740991").get<int64_t>() == -9007199254740991);
					// (2**53)-1
					REX_CHECK(parser_helper("9007199254740991").get<int64_t>() == 9007199254740991);
				}

				SECTION("SECTION - over the edge cases")  // issue #178 - Integer conversion to unsigned (incorrect handling of 64-bit integers)
				{
					// While RFC8259, Section 6 specifies a preference for support
					// for ranges in range of IEEE 754-2008 binary64 (double precision)
					// this does not accommodate 64-bit integers without loss of accuracy.
					// As 64-bit integers are now widely used in software, it is desirable
					// to expand support to the full 64 bit (signed and unsigned) range
					// i.e. -(2**63) -> (2**64)-1.

					// -(2**63)    ** Note: compilers see negative literals as negated positive numbers (hence the -1))
					REX_CHECK(parser_helper("-9223372036854775808").get<int64_t>() == -9223372036854775807 - 1);
					// (2**63)-1
					REX_CHECK(parser_helper("9223372036854775807").get<int64_t>() == 9223372036854775807);
					// (2**64)-1
					REX_CHECK(parser_helper("18446744073709551615").get<uint64_t>() == 18446744073709551615u);
				}
			}

			SECTION("SECTION - floating-point")
			{
				SECTION("SECTION - without exponent")
				{
					REX_CHECK(parser_helper("-128.5") == rex::json::json(-128.5));
					REX_CHECK(parser_helper("0.999") == rex::json::json(0.999));
					REX_CHECK(parser_helper("128.5") == rex::json::json(128.5));
					REX_CHECK(parser_helper("-0.0") == rex::json::json(-0.0));
				}

				SECTION("SECTION - with exponent")
				{
					REX_CHECK(parser_helper("-128.5E3") == rex::json::json(-128.5E3));
					REX_CHECK(parser_helper("-128.5E-3") == rex::json::json(-128.5E-3));
					REX_CHECK(parser_helper("-0.0e1") == rex::json::json(-0.0e1));
					REX_CHECK(parser_helper("-0.0E1") == rex::json::json(-0.0e1));
				}
			}

			SECTION("SECTION - overflow")
			{
				// overflows during parsing yield an exception
				REX_CHECK(parser_helper("1.18973e+4932").is_discarded());
			}

			SECTION("SECTION - invalid numbers")
			{
				// numbers must not begin with "+"
				REX_CHECK(parser_helper("+1").is_discarded() == true);
				REX_CHECK(parser_helper("+0").is_discarded() == true);

				REX_CHECK(parser_helper("01").is_discarded() == true);
				REX_CHECK(parser_helper("-01").is_discarded() == true);
				REX_CHECK(parser_helper("--1").is_discarded() == true);
				REX_CHECK(parser_helper("1.").is_discarded() == true);
				REX_CHECK(parser_helper("1E").is_discarded() == true);
				REX_CHECK(parser_helper("1E-").is_discarded() == true);
				REX_CHECK(parser_helper("1.E1").is_discarded() == true);
				REX_CHECK(parser_helper("-1E").is_discarded() == true);
				REX_CHECK(parser_helper("-0E#").is_discarded() == true);
				REX_CHECK(parser_helper("-0E-#").is_discarded() == true);
				REX_CHECK(parser_helper("-0#").is_discarded() == true);
				REX_CHECK(parser_helper("-0.0:").is_discarded() == true);
				REX_CHECK(parser_helper("-0.0Z").is_discarded() == true);
				REX_CHECK(parser_helper("-0E123:").is_discarded() == true);
				REX_CHECK(parser_helper("-0e0-:").is_discarded() == true);
				REX_CHECK(parser_helper("-0e-:").is_discarded() == true);
				REX_CHECK(parser_helper("-0f").is_discarded() == true);
			}
		}
	}

	SECTION("SECTION - accept")
	{
		SECTION("SECTION - null")
		{
			REX_CHECK(accept_helper("null"));
		}

		SECTION("SECTION - true")
		{
			REX_CHECK(accept_helper("true"));
		}

		SECTION("SECTION - false")
		{
			REX_CHECK(accept_helper("false"));
		}

		SECTION("SECTION - array")
		{
			SECTION("SECTION - empty array")
			{
				REX_CHECK(accept_helper("[]"));
				REX_CHECK(accept_helper("[ ]"));
			}

			SECTION("SECTION - nonempty array")
			{
				REX_CHECK(accept_helper("[true, false, null]"));
			}
		}

		SECTION("SECTION - object")
		{
			SECTION("SECTION - empty object")
			{
				REX_CHECK(accept_helper("{}"));
				REX_CHECK(accept_helper("{ }"));
			}

			SECTION("SECTION - nonempty object")
			{
				REX_CHECK(accept_helper("{\"\": true, \"one\": 1, \"two\": null}"));
			}
		}

		SECTION("SECTION - string")
		{
			// empty string
			REX_CHECK(accept_helper("\"\""));

			SECTION("SECTION - errors")
			{
				// error: tab in string
				REX_CHECK(accept_helper("\"\t\"") == false);
				// error: newline in string
				REX_CHECK(accept_helper("\"\n\"") == false);
				REX_CHECK(accept_helper("\"\r\"") == false);
				// error: backspace in string
				REX_CHECK(accept_helper("\"\b\"") == false);
				// improve code coverage
				REX_CHECK(accept_helper("\uFF01") == false);
				REX_CHECK(accept_helper("[-4:1,]") == false);
				// unescaped control characters
				REX_CHECK(accept_helper("\"\x00\"") == false); // NOLINT(bugprone-string-literal-with-embedded-nul)
				REX_CHECK(accept_helper("\"\x01\"") == false);
				REX_CHECK(accept_helper("\"\x02\"") == false);
				REX_CHECK(accept_helper("\"\x03\"") == false);
				REX_CHECK(accept_helper("\"\x04\"") == false);
				REX_CHECK(accept_helper("\"\x05\"") == false);
				REX_CHECK(accept_helper("\"\x06\"") == false);
				REX_CHECK(accept_helper("\"\x07\"") == false);
				REX_CHECK(accept_helper("\"\x08\"") == false);
				REX_CHECK(accept_helper("\"\x09\"") == false);
				REX_CHECK(accept_helper("\"\x0a\"") == false);
				REX_CHECK(accept_helper("\"\x0b\"") == false);
				REX_CHECK(accept_helper("\"\x0c\"") == false);
				REX_CHECK(accept_helper("\"\x0d\"") == false);
				REX_CHECK(accept_helper("\"\x0e\"") == false);
				REX_CHECK(accept_helper("\"\x0f\"") == false);
				REX_CHECK(accept_helper("\"\x10\"") == false);
				REX_CHECK(accept_helper("\"\x11\"") == false);
				REX_CHECK(accept_helper("\"\x12\"") == false);
				REX_CHECK(accept_helper("\"\x13\"") == false);
				REX_CHECK(accept_helper("\"\x14\"") == false);
				REX_CHECK(accept_helper("\"\x15\"") == false);
				REX_CHECK(accept_helper("\"\x16\"") == false);
				REX_CHECK(accept_helper("\"\x17\"") == false);
				REX_CHECK(accept_helper("\"\x18\"") == false);
				REX_CHECK(accept_helper("\"\x19\"") == false);
				REX_CHECK(accept_helper("\"\x1a\"") == false);
				REX_CHECK(accept_helper("\"\x1b\"") == false);
				REX_CHECK(accept_helper("\"\x1c\"") == false);
				REX_CHECK(accept_helper("\"\x1d\"") == false);
				REX_CHECK(accept_helper("\"\x1e\"") == false);
				REX_CHECK(accept_helper("\"\x1f\"") == false);
			}

			SECTION("SECTION - escaped")
			{
				// quotation mark "\""
				auto r1 = R"("\"")"_json;
				REX_CHECK(accept_helper("\"\\\"\""));
				// reverse solidus "\\"
				auto r2 = R"("\\")"_json;
				REX_CHECK(accept_helper("\"\\\\\""));
				// solidus
				REX_CHECK(accept_helper("\"\\/\""));
				// backspace
				REX_CHECK(accept_helper("\"\\b\""));
				// formfeed
				REX_CHECK(accept_helper("\"\\f\""));
				// newline
				REX_CHECK(accept_helper("\"\\n\""));
				// carriage return
				REX_CHECK(accept_helper("\"\\r\""));
				// horizontal tab
				REX_CHECK(accept_helper("\"\\t\""));

				REX_CHECK(accept_helper("\"\\u0001\""));
				REX_CHECK(accept_helper("\"\\u000a\""));
				REX_CHECK(accept_helper("\"\\u00b0\""));
				REX_CHECK(accept_helper("\"\\u0c00\""));
				REX_CHECK(accept_helper("\"\\ud000\""));
				REX_CHECK(accept_helper("\"\\u000E\""));
				REX_CHECK(accept_helper("\"\\u00F0\""));
				REX_CHECK(accept_helper("\"\\u0100\""));
				REX_CHECK(accept_helper("\"\\u2000\""));
				REX_CHECK(accept_helper("\"\\uFFFF\""));
				REX_CHECK(accept_helper("\"\\u20AC\""));
				REX_CHECK(accept_helper("\"€\""));
				REX_CHECK(accept_helper("\"🎈\""));

				REX_CHECK(accept_helper("\"\\ud80c\\udc60\""));
				REX_CHECK(accept_helper("\"\\ud83c\\udf1e\""));
			}
		}

		SECTION("SECTION - number")
		{
			SECTION("SECTION - integers")
			{
				SECTION("SECTION - without exponent")
				{
					REX_CHECK(accept_helper("-128"));
					REX_CHECK(accept_helper("-0"));
					REX_CHECK(accept_helper("0"));
					REX_CHECK(accept_helper("128"));
				}

				SECTION("SECTION - with exponent")
				{
					REX_CHECK(accept_helper("0e1"));
					REX_CHECK(accept_helper("0E1"));

					REX_CHECK(accept_helper("10000E-4"));
					REX_CHECK(accept_helper("10000E-3"));
					REX_CHECK(accept_helper("10000E-2"));
					REX_CHECK(accept_helper("10000E-1"));
					REX_CHECK(accept_helper("10000E0"));
					REX_CHECK(accept_helper("10000E1"));
					REX_CHECK(accept_helper("10000E2"));
					REX_CHECK(accept_helper("10000E3"));
					REX_CHECK(accept_helper("10000E4"));

					REX_CHECK(accept_helper("10000e-4"));
					REX_CHECK(accept_helper("10000e-3"));
					REX_CHECK(accept_helper("10000e-2"));
					REX_CHECK(accept_helper("10000e-1"));
					REX_CHECK(accept_helper("10000e0"));
					REX_CHECK(accept_helper("10000e1"));
					REX_CHECK(accept_helper("10000e2"));
					REX_CHECK(accept_helper("10000e3"));
					REX_CHECK(accept_helper("10000e4"));

					REX_CHECK(accept_helper("-0e1"));
					REX_CHECK(accept_helper("-0E1"));
					REX_CHECK(accept_helper("-0E123"));
				}

				SECTION("SECTION - edge cases")
				{
					// From RFC8259, Section 6:
					// Note that when such software is used, numbers that are
					// integers and are in the range [-(2**53)+1, (2**53)-1]
					// are interoperable in the sense that implementations will
					// agree exactly on their numeric values.

					// -(2**53)+1
					REX_CHECK(accept_helper("-9007199254740991"));
					// (2**53)-1
					REX_CHECK(accept_helper("9007199254740991"));
				}

				SECTION("SECTION - over the edge cases")  // issue #178 - Integer conversion to unsigned (incorrect handling of 64-bit integers)
				{
					// While RFC8259, Section 6 specifies a preference for support
					// for ranges in range of IEEE 754-2008 binary64 (double precision)
					// this does not accommodate 64 bit integers without loss of accuracy.
					// As 64 bit integers are now widely used in software, it is desirable
					// to expand support to to the full 64 bit (signed and unsigned) range
					// i.e. -(2**63) -> (2**64)-1.

					// -(2**63)    ** Note: compilers see negative literals as negated positive numbers (hence the -1))
					REX_CHECK(accept_helper("-9223372036854775808"));
					// (2**63)-1
					REX_CHECK(accept_helper("9223372036854775807"));
					// (2**64)-1
					REX_CHECK(accept_helper("18446744073709551615"));
				}
			}

			SECTION("SECTION - floating-point")
			{
				SECTION("SECTION - without exponent")
				{
					REX_CHECK(accept_helper("-128.5"));
					REX_CHECK(accept_helper("0.999"));
					REX_CHECK(accept_helper("128.5"));
					REX_CHECK(accept_helper("-0.0"));
				}

				SECTION("SECTION - with exponent")
				{
					REX_CHECK(accept_helper("-128.5E3"));
					REX_CHECK(accept_helper("-128.5E-3"));
					REX_CHECK(accept_helper("-0.0e1"));
					REX_CHECK(accept_helper("-0.0E1"));
				}
			}

			SECTION("SECTION - overflow")
			{
				// overflows during parsing
				REX_CHECK(!accept_helper("1.18973e+4932"));
			}

			SECTION("SECTION - invalid numbers")
			{
				REX_CHECK(accept_helper("01") == false);
				REX_CHECK(accept_helper("--1") == false);
				REX_CHECK(accept_helper("1.") == false);
				REX_CHECK(accept_helper("1E") == false);
				REX_CHECK(accept_helper("1E-") == false);
				REX_CHECK(accept_helper("1.E1") == false);
				REX_CHECK(accept_helper("-1E") == false);
				REX_CHECK(accept_helper("-0E#") == false);
				REX_CHECK(accept_helper("-0E-#") == false);
				REX_CHECK(accept_helper("-0#") == false);
				REX_CHECK(accept_helper("-0.0:") == false);
				REX_CHECK(accept_helper("-0.0Z") == false);
				REX_CHECK(accept_helper("-0E123:") == false);
				REX_CHECK(accept_helper("-0e0-:") == false);
				REX_CHECK(accept_helper("-0e-:") == false);
				REX_CHECK(accept_helper("-0f") == false);

				// numbers must not begin with "+"
				REX_CHECK(accept_helper("+1") == false);
				REX_CHECK(accept_helper("+0") == false);
			}
		}
	}

	SECTION("SECTION - parse errors")
	{
		// unexpected end of number
		REX_CHECK(parser_helper("0.").is_discarded() == true);
		REX_CHECK(parser_helper("-").is_discarded() == true);
		REX_CHECK(parser_helper("--").is_discarded() == true);
		REX_CHECK(parser_helper("-0.").is_discarded() == true);
		REX_CHECK(parser_helper("-.").is_discarded() == true);
		REX_CHECK(parser_helper("-:").is_discarded() == true);
		REX_CHECK(parser_helper("0.:").is_discarded() == true);
		REX_CHECK(parser_helper("e.").is_discarded() == true);
		REX_CHECK(parser_helper("1e.").is_discarded() == true);
		REX_CHECK(parser_helper("1e/").is_discarded() == true);
		REX_CHECK(parser_helper("1e:").is_discarded() == true);
		REX_CHECK(parser_helper("1E.").is_discarded() == true);
		REX_CHECK(parser_helper("1E/").is_discarded() == true);
		REX_CHECK(parser_helper("1E:").is_discarded() == true);

		// unexpected end of null
		REX_CHECK(parser_helper("n").is_discarded() == true);
		REX_CHECK(parser_helper("nu").is_discarded() == true);
		REX_CHECK(parser_helper("nul").is_discarded() == true);
		REX_CHECK(parser_helper("nulk").is_discarded() == true);
		REX_CHECK(parser_helper("nulm").is_discarded() == true);

		// unexpected end of true
		REX_CHECK(parser_helper("t").is_discarded() == true);
		REX_CHECK(parser_helper("tr").is_discarded() == true);
		REX_CHECK(parser_helper("tru").is_discarded() == true);
		REX_CHECK(parser_helper("trud").is_discarded() == true);
		REX_CHECK(parser_helper("truf").is_discarded() == true);

		// unexpected end of false
		REX_CHECK(parser_helper("f").is_discarded() == true);
		REX_CHECK(parser_helper("fa").is_discarded() == true);
		REX_CHECK(parser_helper("fal").is_discarded() == true);
		REX_CHECK(parser_helper("fals").is_discarded() == true);
		REX_CHECK(parser_helper("falsd").is_discarded() == true);
		REX_CHECK(parser_helper("falsf").is_discarded() == true);

		// missing/unexpected end of array
		REX_CHECK(parser_helper("[").is_discarded() == true);
		REX_CHECK(parser_helper("[1").is_discarded() == true);
		REX_CHECK(parser_helper("[1,").is_discarded() == true);
		REX_CHECK(parser_helper("[1,]").is_discarded() == true);
		REX_CHECK(parser_helper("]").is_discarded() == true);

		// missing/unexpected end of object
		REX_CHECK(parser_helper("{").is_discarded() == true);
		REX_CHECK(parser_helper("{\"foo\"").is_discarded() == true);
		REX_CHECK(parser_helper("{\"foo\":").is_discarded() == true);
		REX_CHECK(parser_helper("{\"foo\":}").is_discarded() == true);
		REX_CHECK(parser_helper("{\"foo\":1,}").is_discarded() == true);
		REX_CHECK(parser_helper("}").is_discarded() == true);

		// missing/unexpected end of string
		REX_CHECK(parser_helper("\"").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\\"").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\u\"").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\u0\"").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\u01\"").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\u012\"").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\u").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\u0").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\u01").is_discarded() == true);
		REX_CHECK(parser_helper("\"\\u012").is_discarded() == true);

		// invalid escapes
		for (int c = 1; c < 128; ++c)
		{
			auto s = rsl::string("\"\\") + rsl::string(1, static_cast<char>(c)) + "\"";

			switch (c)
			{
				// valid escapes
			case ('"'):
			case ('\\'):
			case ('/'):
			case ('b'):
			case ('f'):
			case ('n'):
			case ('r'):
			case ('t'):
			{
				REX_CHECK_NOTHROW(parser_helper(s));
				break;
			}

			// \u must be followed with four numbers, so we skip it here
			case ('u'):
			{
				break;
			}

			// any other combination of backslash and character is invalid
			default:
			{
				REX_CHECK(parser_helper(s).is_discarded() == true);
				// only check error message if c is not a control character
				if (c > 0x1f)
				{
					REX_CHECK(parser_helper(s).is_discarded() == true);
				}
				break;
			}
			}
		}

		// invalid \uxxxx escapes
		{
			// check whether character is a valid hex character
			const auto valid = [](int c)
			{
				switch (c)
				{
				case ('0'):
				case ('1'):
				case ('2'):
				case ('3'):
				case ('4'):
				case ('5'):
				case ('6'):
				case ('7'):
				case ('8'):
				case ('9'):
				case ('a'):
				case ('b'):
				case ('c'):
				case ('d'):
				case ('e'):
				case ('f'):
				case ('A'):
				case ('B'):
				case ('C'):
				case ('D'):
				case ('E'):
				case ('F'):
				{
					return true;
				}

				default:
				{
					return false;
				}
				}
			};

			for (int c = 1; c < 128; ++c)
			{
				rsl::string const s("\"\\u");

				// create a string with the iterated character at each position
				auto s1 = s + "000" + rsl::string(1, static_cast<char>(c)) + "\"";
				auto s2 = s + "00" + rsl::string(1, static_cast<char>(c)) + "0\"";
				auto s3 = s + "0" + rsl::string(1, static_cast<char>(c)) + "00\"";
				auto s4 = s + rsl::string(1, static_cast<char>(c)) + "000\"";

				if (valid(c))
				{
					CAPTURE(s1);
					REX_CHECK_NOTHROW(parser_helper(s1).is_discarded() == false);
					CAPTURE(s2);
					REX_CHECK_NOTHROW(parser_helper(s2).is_discarded() == false);
					CAPTURE(s3);
					REX_CHECK_NOTHROW(parser_helper(s3).is_discarded() == false);
					CAPTURE(s4);
					REX_CHECK_NOTHROW(parser_helper(s4).is_discarded() == false);
				}
				else
				{
					CAPTURE(s1);
					REX_CHECK(parser_helper(s1).is_discarded() == true);
					// only check error message if c is not a control character
					if (c > 0x1f)
					{
						REX_CHECK(parser_helper(s1).is_discarded() == true);
					}

					CAPTURE(s2);
					REX_CHECK(parser_helper(s2).is_discarded() == true);
					// only check error message if c is not a control character
					if (c > 0x1f)
					{
						REX_CHECK(parser_helper(s2).is_discarded() == true);
					}

					CAPTURE(s3);
					REX_CHECK(parser_helper(s3).is_discarded() == true);
					// only check error message if c is not a control character
					if (c > 0x1f)
					{
						REX_CHECK(parser_helper(s3).is_discarded() == true);
					}

					CAPTURE(s4);
					REX_CHECK(parser_helper(s4).is_discarded() == true);
					// only check error message if c is not a control character
					if (c > 0x1f)
					{
						REX_CHECK(parser_helper(s4).is_discarded() == true);
					}
				}
			}
		}

		rex::json::json _;

		// missing part of a surrogate pair
		REX_CHECK(rex::json::json::parse("\"\\uD80C\"").is_discarded() == true);
		// invalid surrogate pair
		REX_CHECK(rex::json::json::parse("\"\\uD80C\\uD80C\"").is_discarded() == true);
		REX_CHECK(rex::json::json::parse("\"\\uD80C\\u0000\"").is_discarded() == true);
		REX_CHECK(rex::json::json::parse("\"\\uD80C\\uFFFF\"").is_discarded() == true);
	}

	SECTION("SECTION - parse errors (accept)")
	{
		// unexpected end of number
		REX_CHECK(accept_helper("0.") == false);
		REX_CHECK(accept_helper("-") == false);
		REX_CHECK(accept_helper("--") == false);
		REX_CHECK(accept_helper("-0.") == false);
		REX_CHECK(accept_helper("-.") == false);
		REX_CHECK(accept_helper("-:") == false);
		REX_CHECK(accept_helper("0.:") == false);
		REX_CHECK(accept_helper("e.") == false);
		REX_CHECK(accept_helper("1e.") == false);
		REX_CHECK(accept_helper("1e/") == false);
		REX_CHECK(accept_helper("1e:") == false);
		REX_CHECK(accept_helper("1E.") == false);
		REX_CHECK(accept_helper("1E/") == false);
		REX_CHECK(accept_helper("1E:") == false);

		// unexpected end of null
		REX_CHECK(accept_helper("n") == false);
		REX_CHECK(accept_helper("nu") == false);
		REX_CHECK(accept_helper("nul") == false);

		// unexpected end of true
		REX_CHECK(accept_helper("t") == false);
		REX_CHECK(accept_helper("tr") == false);
		REX_CHECK(accept_helper("tru") == false);

		// unexpected end of false
		REX_CHECK(accept_helper("f") == false);
		REX_CHECK(accept_helper("fa") == false);
		REX_CHECK(accept_helper("fal") == false);
		REX_CHECK(accept_helper("fals") == false);

		// missing/unexpected end of array
		REX_CHECK(accept_helper("[") == false);
		REX_CHECK(accept_helper("[1") == false);
		REX_CHECK(accept_helper("[1,") == false);
		REX_CHECK(accept_helper("[1,]") == false);
		REX_CHECK(accept_helper("]") == false);

		// missing/unexpected end of object
		REX_CHECK(accept_helper("{") == false);
		REX_CHECK(accept_helper("{\"foo\"") == false);
		REX_CHECK(accept_helper("{\"foo\":") == false);
		REX_CHECK(accept_helper("{\"foo\":}") == false);
		REX_CHECK(accept_helper("{\"foo\":1,}") == false);
		REX_CHECK(accept_helper("}") == false);

		// missing/unexpected end of string
		REX_CHECK(accept_helper("\"") == false);
		REX_CHECK(accept_helper("\"\\\"") == false);
		REX_CHECK(accept_helper("\"\\u\"") == false);
		REX_CHECK(accept_helper("\"\\u0\"") == false);
		REX_CHECK(accept_helper("\"\\u01\"") == false);
		REX_CHECK(accept_helper("\"\\u012\"") == false);
		REX_CHECK(accept_helper("\"\\u") == false);
		REX_CHECK(accept_helper("\"\\u0") == false);
		REX_CHECK(accept_helper("\"\\u01") == false);
		REX_CHECK(accept_helper("\"\\u012") == false);

		// unget of newline
		REX_CHECK(parser_helper("\n123\n") == 123);

		// invalid escapes
		for (int c = 1; c < 128; ++c)
		{
			auto s = rsl::string("\"\\") + rsl::string(1, static_cast<char>(c)) + "\"";

			switch (c)
			{
				// valid escapes
			case ('"'):
			case ('\\'):
			case ('/'):
			case ('b'):
			case ('f'):
			case ('n'):
			case ('r'):
			case ('t'):
			{
				REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s)).accept());
				break;
			}

			// \u must be followed with four numbers, so we skip it here
			case ('u'):
			{
				break;
			}

			// any other combination of backslash and character is invalid
			default:
			{
				REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s)).accept() == false);
				break;
			}
			}
		}

		// invalid \uxxxx escapes
		{
			// check whether character is a valid hex character
			const auto valid = [](int c)
			{
				switch (c)
				{
				case ('0'):
				case ('1'):
				case ('2'):
				case ('3'):
				case ('4'):
				case ('5'):
				case ('6'):
				case ('7'):
				case ('8'):
				case ('9'):
				case ('a'):
				case ('b'):
				case ('c'):
				case ('d'):
				case ('e'):
				case ('f'):
				case ('A'):
				case ('B'):
				case ('C'):
				case ('D'):
				case ('E'):
				case ('F'):
				{
					return true;
				}

				default:
				{
					return false;
				}
				}
			};

			for (int c = 1; c < 128; ++c)
			{
				rsl::string const s("\"\\u");

				// create a string with the iterated character at each position
				const auto s1 = s + "000" + rsl::string(1, static_cast<char>(c)) + "\"";
				const auto s2 = s + "00" + rsl::string(1, static_cast<char>(c)) + "0\"";
				const auto s3 = s + "0" + rsl::string(1, static_cast<char>(c)) + "00\"";
				const auto s4 = s + rsl::string(1, static_cast<char>(c)) + "000\"";

				if (valid(c))
				{
					CAPTURE(s1);
					REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s1)).accept());
					
					CAPTURE(s2);
					REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s2)).accept());
					
					CAPTURE(s3);
					REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s3)).accept());
					
					CAPTURE(s4);
					REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s4)).accept());
				}
				else
				{
					CAPTURE(s1);
					REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s1)).accept() == false);

					CAPTURE(s2);
					REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s2)).accept() == false);

					CAPTURE(s3);
					REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s3)).accept() == false);

					CAPTURE(s4);
					REX_CHECK(rex::json::json::parser(nlohmann::detail::input_adapter(s4)).accept() == false);
				}
			}
		}

		// missing part of a surrogate pair
		REX_CHECK(accept_helper("\"\\uD80C\"") == false);
		// invalid surrogate pair
		REX_CHECK(accept_helper("\"\\uD80C\\uD80C\"") == false);
		REX_CHECK(accept_helper("\"\\uD80C\\u0000\"") == false);
		REX_CHECK(accept_helper("\"\\uD80C\\uFFFF\"") == false);
	}

	SECTION("SECTION - tests found by mutate++")
	{
		// test case to make sure no comma precedes the first key
		REX_CHECK(parser_helper("{,\"key\": false}").is_discarded() == true);
		// test case to make sure an object is properly closed
		REX_CHECK(parser_helper("[{\"key\": false true]").is_discarded() == true);

		// test case to make sure the callback is properly evaluated after reading a key
		{
			rex::json::json::parser_callback_t const cb = [](int /*unused*/, rex::json::json::parse_event_t event, rex::json::json& /*unused*/) noexcept
			{
				return event != rex::json::json::parse_event_t::key;
			};

			rex::json::json x = rex::json::json::parse("{\"key\": false}", cb);
			REX_CHECK(x == rex::json::json::object());
		}
	}

	SECTION("SECTION - callback function")
	{
		const auto* s_object = R"(
            {
                "foo": 2,
                "bar": {
                    "baz": 1
                }
            }
        )";

		const auto* s_array = R"(
            [1,2,[3,4,5],4,5]
        )";

		const auto* structured_array = R"(
            [
                1,
                {
                     "foo": "bar"
                },
                {
                     "qux": "baz"
                }
            ]
        )";

		SECTION("SECTION - filter nothing")
		{
			rex::json::json j_object = rex::json::json::parse(s_object, [](int /*unused*/, rex::json::json::parse_event_t /*unused*/, const rex::json::json& /*unused*/) noexcept
				{
					return true;
				});

			REX_CHECK(j_object == rex::json::json({ {"foo", 2}, {"bar", {{"baz", 1}}} }));

			rex::json::json j_array = rex::json::json::parse(s_array, [](int /*unused*/, rex::json::json::parse_event_t /*unused*/, const rex::json::json& /*unused*/) noexcept
				{
					return true;
				});

			REX_CHECK(j_array == rex::json::json({ 1, 2, {3, 4, 5}, 4, 5 }));
		}

		SECTION("SECTION - filter everything")
		{
			rex::json::json const j_object = rex::json::json::parse(s_object, [](int /*unused*/, rex::json::json::parse_event_t /*unused*/, const rex::json::json& /*unused*/) noexcept
				{
					return false;
				});

			// the top-level object will be discarded, leaving a null
			REX_CHECK(j_object.is_null());

			rex::json::json const j_array = rex::json::json::parse(s_array, [](int /*unused*/, rex::json::json::parse_event_t /*unused*/, const rex::json::json& /*unused*/) noexcept
				{
					return false;
				});

			// the top-level array will be discarded, leaving a null
			REX_CHECK(j_array.is_null());
		}

		SECTION("SECTION - filter specific element")
		{
			rex::json::json j_object = rex::json::json::parse(s_object, [](int /*unused*/, rex::json::json::parse_event_t event, const rex::json::json& j) noexcept
				{
					// filter all number(2) elements
					return event != rex::json::json::parse_event_t::value || j != rex::json::json(2);
				});

			REX_CHECK(j_object == rex::json::json({ {"bar", {{"baz", 1}}} }));

			rex::json::json j_array = rex::json::json::parse(s_array, [](int /*unused*/, rex::json::json::parse_event_t event, const rex::json::json& j) noexcept
				{
					return event != rex::json::json::parse_event_t::value || j != rex::json::json(2);
				});

			REX_CHECK(j_array == rex::json::json({ 1, {3, 4, 5}, 4, 5 }));
		}

		SECTION("SECTION - filter object in array")
		{
			rex::json::json j_filtered1 = rex::json::json::parse(structured_array, [](int /*unused*/, rex::json::json::parse_event_t e, const rex::json::json& parsed)
				{
					return !(e == rex::json::json::parse_event_t::object_end && parsed.contains("foo"));
				});

			// the specified object will be discarded, and removed.
			REX_CHECK(j_filtered1.size() == 2);
			REX_CHECK(j_filtered1 == rex::json::json({ 1, {{"qux", "baz"}} }));

			rex::json::json j_filtered2 = rex::json::json::parse(structured_array, [](int /*unused*/, rex::json::json::parse_event_t e, const rex::json::json& /*parsed*/) noexcept
				{
					return e != rex::json::json::parse_event_t::object_end;
				});

			// removed all objects in array.
			REX_CHECK(j_filtered2.size() == 1);
			REX_CHECK(j_filtered2 == rex::json::json({ 1 }));
		}

		SECTION("SECTION - filter specific events")
		{
			SECTION("SECTION - first closing event")
			{
				{
					rex::json::json j_object = rex::json::json::parse(s_object, [](int /*unused*/, rex::json::json::parse_event_t e, const rex::json::json& /*unused*/) noexcept
						{
							static bool first = true;
							if (e == rex::json::json::parse_event_t::object_end && first)
							{
								first = false;
								return false;
							}

							return true;
						});

					// the first completed object will be discarded
					REX_CHECK(j_object == rex::json::json({ {"foo", 2} }));
				}

				{
					rex::json::json j_array = rex::json::json::parse(s_array, [](int /*unused*/, rex::json::json::parse_event_t e, const rex::json::json& /*unused*/) noexcept
						{
							static bool first = true;
							if (e == rex::json::json::parse_event_t::array_end && first)
							{
								first = false;
								return false;
							}

							return true;
						});

					// the first completed array will be discarded
					REX_CHECK(j_array == rex::json::json({ 1, 2, 4, 5 }));
				}
			}
		}

		SECTION("SECTION - special cases")
		{
			// the following test cases cover the situation in which an empty
			// object and array is discarded only after the closing character
			// has been read

			rex::json::json j_empty_object = rex::json::json::parse("{}", [](int /*unused*/, rex::json::json::parse_event_t e, const rex::json::json& /*unused*/) noexcept
				{
					return e != rex::json::json::parse_event_t::object_end;
				});
			REX_CHECK(j_empty_object == rex::json::json());

			rex::json::json j_empty_array = rex::json::json::parse("[]", [](int /*unused*/, rex::json::json::parse_event_t e, const rex::json::json& /*unused*/) noexcept
				{
					return e != rex::json::json::parse_event_t::array_end;
				});
			REX_CHECK(j_empty_array == rex::json::json());
		}
	}

	SECTION("SECTION - constructing from contiguous containers")
	{
		SECTION("SECTION - from rsl::vector")
		{
			rsl::vector<uint8_t> v = { 't', 'r', 'u', 'e' };
			rex::json::json j;
			rex::json::json::parser(nlohmann::detail::input_adapter(rsl::begin(v), rsl::end(v))).parse(true, j);
			REX_CHECK(j == rex::json::json(true));
		}

		SECTION("SECTION - from rsl::array")
		{
			rsl::array<uint8_t, 5> v{ {'t', 'r', 'u', 'e'} };
			rex::json::json j;
			rex::json::json::parser(nlohmann::detail::input_adapter(rsl::begin(v), rsl::end(v))).parse(true, j);
			REX_CHECK(j == rex::json::json(true));
		}

		SECTION("SECTION - from array")
		{
			uint8_t v[] = { 't', 'r', 'u', 'e' }; // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
			rex::json::json j;
			rex::json::json::parser(nlohmann::detail::input_adapter(rsl::begin(v), rsl::end(v))).parse(true, j);
			REX_CHECK(j == rex::json::json(true));
		}

		SECTION("SECTION - from char literal")
		{
			REX_CHECK(parser_helper("true") == rex::json::json(true));
		}

		SECTION("SECTION - from rsl::string")
		{
			rsl::string v = { 't', 'r', 'u', 'e' };
			rex::json::json j;
			rex::json::json::parser(nlohmann::detail::input_adapter(rsl::begin(v), rsl::end(v))).parse(true, j);
			REX_CHECK(j == rex::json::json(true));
		}

		SECTION("SECTION - from rsl::initializer_list")
		{
			rsl::initializer_list<uint8_t> const v = { 't', 'r', 'u', 'e' };
			rex::json::json j;
			rex::json::json::parser(nlohmann::detail::input_adapter(rsl::begin(v), rsl::end(v))).parse(true, j);
			REX_CHECK(j == rex::json::json(true));
		}

		// To implement when valarray is implemented
		//SECTION("SECTION - from rsl::valarray")
		//{
		//	rsl::valarray<uint8_t> v = { 't', 'r', 'u', 'e' };
		//	rex::json::json j;
		//	rex::json::json::parser(nlohmann::detail::input_adapter(rsl::begin(v), rsl::end(v))).parse(true, j);
		//	REX_CHECK(j == rex::json::json(true));
		//}
	}

	SECTION("SECTION - improve test coverage")
	{
		SECTION("SECTION - parser with callback")
		{
			rex::json::json::parser_callback_t const cb = [](int /*unused*/, rex::json::json::parse_event_t /*unused*/, rex::json::json& /*unused*/) noexcept
			{
				return true;
			};

			REX_CHECK(rex::json::json::parse("{\"foo\": true:", cb, false).is_discarded());

			rex::json::json _;
			REX_CHECK(rex::json::json::parse("{\"foo\": true:", cb).is_discarded() == true);

			REX_CHECK(rex::json::json::parse("1.18973e+4932", cb).is_discarded() == true);
		}

		SECTION("SECTION - SAX parser")
		{
			SECTION("SECTION - } without value")
			{
				SaxCountdown s(1);
				REX_CHECK(rex::json::json::sax_parse("{}", &s) == false);
			}

			SECTION("SECTION - } with value")
			{
				SaxCountdown s(3);
				REX_CHECK(rex::json::json::sax_parse("{\"k1\": true}", &s) == false);
			}

			SECTION("SECTION - second key")
			{
				SaxCountdown s(3);
				REX_CHECK(rex::json::json::sax_parse("{\"k1\": true, \"k2\": false}", &s) == false);
			}

			SECTION("SECTION - ] without value")
			{
				SaxCountdown s(1);
				REX_CHECK(rex::json::json::sax_parse("[]", &s) == false);
			}

			SECTION("SECTION - ] with value")
			{
				SaxCountdown s(2);
				REX_CHECK(rex::json::json::sax_parse("[1]", &s) == false);
			}

			SECTION("SECTION - float")
			{
				SaxCountdown s(0);
				REX_CHECK(rex::json::json::sax_parse("3.14", &s) == false);
			}

			SECTION("SECTION - false")
			{
				SaxCountdown s(0);
				REX_CHECK(rex::json::json::sax_parse("false", &s) == false);
			}

			SECTION("SECTION - null")
			{
				SaxCountdown s(0);
				REX_CHECK(rex::json::json::sax_parse("null", &s) == false);
			}

			SECTION("SECTION - true")
			{
				SaxCountdown s(0);
				REX_CHECK(rex::json::json::sax_parse("true", &s) == false);
			}

			SECTION("SECTION - unsigned")
			{
				SaxCountdown s(0);
				REX_CHECK(rex::json::json::sax_parse("12", &s) == false);
			}

			SECTION("SECTION - integer")
			{
				SaxCountdown s(0);
				REX_CHECK(rex::json::json::sax_parse("-12", &s) == false);
			}

			SECTION("SECTION - string")
			{
				SaxCountdown s(0);
				REX_CHECK(rex::json::json::sax_parse("\"foo\"", &s) == false);
			}
		}
	}

	SECTION("SECTION - error messages for comments")
	{
		rex::json::json _;
		REX_CHECK(rex::json::json::parse("/a", nullptr, false, true).is_discarded() == true);
		REX_CHECK(rex::json::json::parse("/*", nullptr, false, true).is_discarded() == true);
	}
}
