#include "rex_engine/images/image_loader.h"

#include "rex_engine/images/internal/stb_image.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/engine/defines.h"

namespace rex
{
	ImageLoadResult load_image(rsl::string_view filepath)
	{
		REX_ASSERT_X(rex::is_null_terminated(filepath), "filepath for image loading is not null terminated");

		ImageLoadResult res{};
		u8* data_raw = stbi_load(filepath.data(), &res.width, &res.height, &res.num_channels, 0);

		REX_STATIC_WARNING("Implement realloc functionality so that an image doesn't have to be copied anymore");
		res.data = rsl::make_unique<u8[]>(res.width * res.height * res.num_channels);
		rsl::memcpy(res.data.get(), data_raw, res.data.count());

		return res;
	}
}