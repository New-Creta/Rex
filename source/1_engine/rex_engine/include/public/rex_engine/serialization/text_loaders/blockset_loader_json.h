#pragma once

#include "rex_engine/serialization/asset_loader.h"

#include "rex_std/memory.h"

namespace rex
{
	class Block;

	class BlocksetLoaderJson : public AssetLoader
	{
	public:
		BlocksetLoaderJson();

		rsl::unique_ptr<Asset> load(rsl::string_view assetPath, LoadFlags loadFlags) override;
		void hydrate_asset(Asset* asset, rsl::string_view assetPath) override;

	private:
		rsl::unique_array<Block> load_block_indices(rsl::string_view blockIndicesPath);
	};
}