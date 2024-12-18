#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/engine/casting.h"

TEST_CASE("narrow casting")
{
	s32 a = 0x7FFF;
	s16 b = rex::unsafe_narrow_cast<s16>(a);
	REX_CHECK(a == b);

	s16 c = 0x7F;
	s8 d = rex::unsafe_narrow_cast<s8>(c);
	REX_CHECK(c == d);

	u32 e = 0x7FFF;
	u16 f = rex::unsafe_narrow_cast<s16>(e);
	REX_CHECK(e == f);

	u16 g = 0x7F;
	u8 h = rex::unsafe_narrow_cast<s8>(g);
	REX_CHECK(g == h);

	f64 i = 10.0f;
	f32 j = rex::unsafe_narrow_cast<f32>(i);
	REX_CHECK(i == j);
}

TEST_CASE("sign casting")
{
	s32 a = 0x7FFFFFFF;
	u32 b = rex::unsafe_sign_cast<u32>(a);
	REX_CHECK(a == b);

	s16 c = 0x7FFF;
	u16 d = rex::unsafe_sign_cast<u16>(c);
	REX_CHECK(c == d);

	s8 e = 0x7F;
	u8 f = rex::unsafe_sign_cast<u8>(e);
	REX_CHECK(e == f);

	u32 g = 0x7FFFFFFF;
	s32 h = rex::unsafe_sign_cast<s32>(g);
	REX_CHECK(g == h);

	u16 i = 0x7FFF;
	s16 j = rex::unsafe_sign_cast<s16>(i);
	REX_CHECK(i == j);

	u8 k = 0x7F;
	s8 l = rex::unsafe_sign_cast<s8>(k);
	REX_CHECK(k == l);
}

TEST_CASE("flip sign")
{
	s32 a = 0x7FFFFFFF;
	u32 b = rex::unsafe_flip_sign(a);
	REX_CHECK(a == b);

	s16 c = 0x7FFF;
	u16 d = rex::unsafe_flip_sign(c);
	REX_CHECK(c == d);

	s8 e = 0x7F;
	u8 f = rex::unsafe_flip_sign(e);
	REX_CHECK(e == f);

	u32 g = 0x7FFFFFFF;
	s32 h = rex::unsafe_flip_sign(g);
	REX_CHECK(g == h);

	u16 i = 0x7FFF;
	s16 j = rex::unsafe_flip_sign(i);
	REX_CHECK(i == j);

	u8 k = 0x7F;
	s8 l = rex::unsafe_flip_sign(k);
	REX_CHECK(k == l);
}

TEST_CASE("numeric cast")
{
	// Smaller to larger integer (same sign)
	s8 small_int = 42;
	s32 large_int = rex::numeric_cast<s32>(small_int);
	REX_CHECK(large_int == small_int);

	// Larger to smaller integer (same sign, value within range)
	s32 large_int2 = 100;
	s8 small_int2 = rex::numeric_cast<s8>(large_int2);
	REX_CHECK(small_int2 == large_int2);

	// Unsigned to signed (value within range)
	uint16_t unsigned_int = 300;
	s32 signed_int = rex::numeric_cast<s32>(unsigned_int);
	REX_CHECK(signed_int == unsigned_int);

	// Signed to unsigned (value non-negative)
	int16_t signed_int2 = 200;
	u32 unsigned_int2 = rex::numeric_cast<u32>(signed_int2);
	REX_CHECK(unsigned_int2 == signed_int2);
}

TEST_CASE("byte cast")
{
	s32 x = 0xFFEEDDCC;

	rsl::byte* bytes = rex::byte_cast(&x);
	REX_CHECK(bytes[0] == static_cast<rsl::byte>(0xCC));
	REX_CHECK(bytes[1] == static_cast<rsl::byte>(0xDD));
	REX_CHECK(bytes[2] == static_cast<rsl::byte>(0xEE));
	REX_CHECK(bytes[3] == static_cast<rsl::byte>(0xFF));
}

TEST_CASE("char cast")
{
	s32 x = 0xFFEEDDCC;

	rsl::char8* bytes = rex::char_cast(&x);
	REX_CHECK(bytes[0] == static_cast<char8>(0xCC));
	REX_CHECK(bytes[1] == static_cast<char8>(0xDD));
	REX_CHECK(bytes[2] == static_cast<char8>(0xEE));
	REX_CHECK(bytes[3] == static_cast<char8>(0xFF));
}