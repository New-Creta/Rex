#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/memory.h"
#include "rex_std/string_view.h"

namespace rex
{
	struct ImageLoadResult
	{
		rsl::unique_array<u8> data;
		s32 width;
		s32 height;
		s32 num_channels;
	};
	ImageLoadResult load_image(rsl::string_view filepath);
}