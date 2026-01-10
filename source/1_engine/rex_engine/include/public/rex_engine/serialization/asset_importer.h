#pragma once

#include "rex_engine/serialization/asset_loader.h"

#include "rex_std/vector.h"
#include "rex_std/memory.h"

namespace rex
{
	enum class LoadFlags;

	// An asset importer is a wrapper around asset loaders
	// Given an asset path, it will look in its internal loaders
	// if any of them can load the asset at the specified filepath
	class AssetImporter
	{
	public:
		void add_loader(rsl::unique_ptr<AssetLoader> loader);

		bool can_load(rsl::string_view assetPath) const;
		rsl::vector<rsl::string_view> allowed_extensions() const;
		rsl::unique_ptr<Asset> load(rsl::string_view assetPath, LoadFlags loadFlags);
		void hydrate_asset(Asset* asset, rsl::string_view assetPath);

	private:
		rsl::vector<rsl::unique_ptr<AssetLoader>> m_loaders;
	};
}