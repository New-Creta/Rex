#pragma once

#include "rex_engine/serialization/asset_loader.h"

namespace rex
{
	class InputMappingLoaderJson : public AssetLoader
	{
	public:
		InputMappingLoaderJson();

		rsl::unique_ptr<Asset> load(rsl::string_view assetPath, LoadFlags loadFlags) override;
	};
}