#pragma once

#include "rex_std/bonus/math/point.h"

#include "rex_engine/engine/types.h"

namespace rex
{
	template <typename T>
	struct TRect
	{
		rsl::point<T> top_left;
		rsl::point<T> bottom_right;
	};

	using Rect8 = TRect<s8>;
}