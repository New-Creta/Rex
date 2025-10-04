#pragma once

#include "rex_engine/serialization/asset_loader.h"

namespace rex
{
	class TilesetLoaderJson : public AssetLoader
	{
	public:
		TilesetLoaderJson();

		rsl::unique_ptr<Asset> load(rsl::string_view assetPath, LoadFlags loadFlags) override;
		void hydrate_asset(Asset* asset, rsl::string_view assetPath) override;

	};
}