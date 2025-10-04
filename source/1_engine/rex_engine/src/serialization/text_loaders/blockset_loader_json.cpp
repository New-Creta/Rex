#include "rex_engine/serialization/text_loaders/blockset_loader_json.h"

#include "rex_engine/memory/blob_reader.h"

#include "rex_engine/text_processing/json.h"
#include "rex_engine/assets/blockset.h"

#include "rex_engine/engine/asset_db.h"

#include "rex_engine/gfx/graphics.h"
#include "rex_engine/gfx/resources/texture_2d.h"

#include "rex_std/bonus/math.h"

namespace rex
{
	BlocksetLoaderJson::BlocksetLoaderJson()
		: AssetLoader(
			{
				rsl::version(0, 0, 1),
				{ ".json" }
			})
	{}

	rsl::unique_ptr<Asset> BlocksetLoaderJson::load(rsl::string_view assetPath, LoadFlags loadFlags)
	{
		rex::json::json json_content = rex::json::read_from_file(assetPath);

		rsl::string_view tileset_path = json_content["tileset"];
		rsl::string_view blockset_path = json_content["blockset"];

		TilesetAsset* tileset = asset_db::instance()->load<TilesetAsset>(tileset_path);
		rsl::unique_array<Block> blocks = load_block_indices(blockset_path);

		return rsl::make_unique<Blockset>(tileset, rsl::move(blocks));
	}
	void BlocksetLoaderJson::hydrate_asset(Asset* asset, rsl::string_view assetPath)
	{
		// Nothing to implement
	}

	rsl::unique_array<Block> BlocksetLoaderJson::load_block_indices(rsl::string_view blockIndicesPath)
	{
		memory::Blob content = vfs::instance()->read_file(blockIndicesPath);

		s64 num_blocks = content.size() / Block::num_tiles();
		memory::BlobReader reader(content);

		Block::indices_array blob_memory;
		rsl::unique_array<Block> blocks = rsl::make_unique<Block[]>(static_cast<s32>(num_blocks));
		for (s32 block_idx = 0; block_idx < num_blocks; ++block_idx)
		{
			reader.read(blob_memory.data(), blob_memory.size());
			blocks[block_idx] = Block(blob_memory);
		}

		return blocks;
	}

}