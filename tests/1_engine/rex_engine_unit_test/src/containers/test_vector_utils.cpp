#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/containers/vector_utils.h"

#include <vector>

TEST_CASE("TEST - Vector Utils - Transfering objects between vectors")
{
	rsl::vector<int> vec1 = { 0, 1, 2 };
	rsl::vector<int> vec2 = { 3, 4, 5 };

	SECTION("SECTION - removing from the front")
	{
		rex::transfer_object_between_vectors(0, vec1, vec2);

		REX_CHECK(vec1 == rsl::vector<int>({ 1, 2 }));
		REX_CHECK(vec2 == rsl::vector{ 3, 4, 5, 0 });
	}

	SECTION("SECTION - removing from the middle")
	{
		rex::transfer_object_between_vectors(1, vec1, vec2);

		REX_CHECK(vec1 == rsl::vector<int>({ 0, 2 }));
		REX_CHECK(vec2 == rsl::vector{ 3, 4, 5, 1 });
	}

	SECTION("SECTION - removing from the end")
	{
		rex::transfer_object_between_vectors(2, vec1, vec2);

		REX_CHECK(vec1 == rsl::vector<int>({ 0, 1 }));
		REX_CHECK(vec2 == rsl::vector{ 3, 4, 5, 2 });
	}

	SECTION("SECTION - removing from various places")
	{
		rex::transfer_object_between_vectors(1, vec1, vec2);

		REX_CHECK(vec1 == rsl::vector<int>({ 0, 2 }));
		REX_CHECK(vec2 == rsl::vector{ 3, 4, 5, 1 });

		rex::transfer_object_between_vectors(0, vec1, vec2);

		REX_CHECK(vec1 == rsl::vector{ 2 });
		REX_CHECK(vec2 == rsl::vector{ 3, 4, 5, 1, 0 });

		rex::transfer_object_between_vectors(0, vec1, vec2);

		REX_CHECK(vec1 == rsl::vector<int>());
		REX_CHECK(vec2 == rsl::vector{ 3, 4, 5, 1, 0, 2 });
	}

}