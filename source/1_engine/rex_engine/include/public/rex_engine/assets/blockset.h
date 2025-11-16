#pragma once

#include "rex_engine/assets/asset.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/assets/tileset_asset.h"

#include "rex_std/memory.h"
#include "rex_std/bonus/math.h"

namespace rex
{
	// A block is a 4x4 matrix of tiles stored row major
	// +-----+-----+-----+-----+
	// |  1  |  2  |  3  |  4  |
	// +-----+-----+-----+-----+
	// |  5  |  6  |  7  |  8  |
	// +-----+-----+-----+-----+
	// |  9  | 10  | 11  | 12  |
	// +-----+-----+-----+-----+
	// | 13  | 14  | 15  | 16  |
	// +-----+-----+-----+-----+
	class Block
	{
	private:
		constexpr static s8 s_num_tiles_per_row = 4;
		constexpr static s8 s_num_tiles_per_column = 4;
		constexpr static s8 s_num_tiles = s_num_tiles_per_row * s_num_tiles_per_column;
		constexpr static u8 s_invalid_tile_index = 255;

	public:
		using indices_array = rsl::array<u8, s_num_tiles>;

		Block();
		Block(const indices_array& tileIndices);

		// set all indices in the block to a value
		void set_all(s8 idx);

		// return the tile index at an index or coordinate
		u8 index_at(s8 idx) const;
		u8 index_at(rsl::pointi8 coord) const;

		// fill in a buffer with tile indices of a single row
		void read_row(s32 rowIdx, u8* dstBuff, s32 dstBuffSize) const;

		// iterator functions to allow for range based for
		rsl::random_access_iterator<u8> begin();
		rsl::const_random_access_iterator<u8> begin() const;
		rsl::const_random_access_iterator<u8> cbegin() const;

		rsl::random_access_iterator<u8> end();
		rsl::const_random_access_iterator<u8> end() const;
		rsl::const_random_access_iterator<u8> cend() const;

		// getters for the static vars
		static s8 num_tiles();
		static s8 num_tiles_per_row();
		static s8 num_tiles_per_column();

	private:
		indices_array m_tile_indices;
	};

	// similar to a tileset, a blockset holds a set of blocks
	// this is done to increase modular design while reducing storage needs on disk
	class Blockset : public Asset
	{
	public:
		Blockset(TilesetAsset* tileset, rsl::unique_array<Block> blocks);

		const Block& block(s32 idx) const;
		const TilesetAsset* tileset() const;

	private:
		TilesetAsset* m_tileset;
		rsl::unique_array<Block> m_blocks;
	};
}