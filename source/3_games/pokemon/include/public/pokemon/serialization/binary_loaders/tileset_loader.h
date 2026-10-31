#pragma once

#include "rex_engine/serialization/asset_loader.h"

namespace pokemon
{
	class TilesetLoader : public rex::AssetLoader
	{
	public:
		TilesetLoader();

		rsl::unique_ptr<rex::Asset> load(rsl::string_view assetPath, rex::LoadFlags loadFlags);
	};
}