#include "rex_engine/serialization/image_loading.h"
#include "rex_engine/serialization/stb_image.h"

#include "rex_engine/engine/defines.h"

namespace rex
{
	ImageLoadResult load_image(memory::BlobView content)
	{
		const rsl::byte* content_data = content.data();
		const unsigned char* data_as_char = reinterpret_cast<const stbi_uc*>(content_data);

		ImageLoadResult res{};
		u8* data_raw = stbi_load_from_memory(data_as_char, content.size(), &res.width, &res.height, &res.num_channels, 0);

		s64 image_size = res.width * res.height * res.num_channels;
		res.data = rsl::unique_array<u8>(data_raw, image_size);

		return res;
	}
}