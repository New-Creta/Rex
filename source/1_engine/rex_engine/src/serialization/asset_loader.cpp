#include "rex_engine/serialization/asset_loader.h"

#include "rex_engine/filesystem/path.h"
#include "rex_std/algorithm.h"

namespace rex
{
	AssetLoader::AssetLoader(AssetLoaderDesc&& desc)
		: m_desc(rsl::move(desc))
	{}

	bool AssetLoader::can_load(rsl::string_view assetPath) const
	{
		return has_allowed_extension(assetPath);
	}

	void AssetLoader::hydrate_asset(Asset* asset, rsl::string_view assetPath)
	{
		REX_UNUSED_PARAM(asset);
		REX_UNUSED_PARAM(assetPath);

		// Nothing to implement
	}

	void AssetLoader::allowed_extensions(rsl::Out<rsl::vector<rsl::string_view>> extensions) const
	{
		rsl::vector<rsl::string_view>& out_extensions = extensions.get();
		out_extensions.reserve(out_extensions.capacity() + m_desc.extensions.size());

		for (rsl::string_view ext : m_desc.extensions)
		{
			out_extensions.push_back(ext);
		}
	}

	bool AssetLoader::has_allowed_extension(rsl::string_view assetPath) const
	{
		rsl::string_view ext = rex::path::extension(assetPath);

		return rsl::find(m_desc.extensions.cbegin(), m_desc.extensions.cend(), ext) != m_desc.extensions.cend();
	}

}