#pragma once

namespace rex
{
	enum class ScrollDirection
	{
		UP,
		DOWN
	};

	struct ScrollMetaData
	{
		ScrollDirection direction;
		int32 wheel_delta;
	};
}