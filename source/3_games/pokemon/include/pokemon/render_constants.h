#pragma once

#include "rex_engine/engine/types.h"

namespace pokemon
{
	namespace constants
	{
		constexpr u8 g_map_padding_blocks = 3;

		constexpr u8 g_num_tiles_per_square_row = 2;
		constexpr u8 g_num_tiles_per_square_column = 2;
		constexpr u8 g_num_tiles_per_square = g_num_tiles_per_square_row * g_num_tiles_per_square_column;

		constexpr u8 g_num_tiles_per_block_row = 4;
		constexpr u8 g_num_tiles_per_block_column = 4;
		constexpr u8 g_num_tiles_per_block = g_num_tiles_per_block_row * g_num_tiles_per_block_column;

		constexpr u8 g_tile_width_px = 8;
		constexpr u8 g_tile_height_px = 8;
		constexpr u8 g_tile_size_px = g_tile_width_px * g_tile_height_px;

		constexpr u8 g_square_width_px = g_num_tiles_per_square_row * g_tile_width_px;
		constexpr u8 g_square_height_px = g_num_tiles_per_square_column * g_tile_height_px;
		constexpr u8 g_square_size_px = g_square_width_px * g_square_height_px;

		constexpr u8 g_block_width_px = g_num_tiles_per_block_row * g_tile_width_px;
		constexpr u8 g_block_height_px = g_num_tiles_per_block_column * g_tile_height_px;
		constexpr u8 g_block_size_px = g_block_width_px * g_block_height_px;

		constexpr rsl::pointi8 player_render_position = { 8,8 };

		constexpr u8 g_screen_width = 160;
		constexpr u8 g_screen_height = 144;

		constexpr u8 g_screen_width_in_tiles = g_screen_width / g_tile_width_px;
		constexpr u8 g_screen_height_in_tiles = g_screen_height / g_tile_height_px;

		constexpr u8 g_screen_width_in_squares = g_screen_width / g_square_width_px;
		constexpr u8 g_screen_height_in_squares = g_screen_height / g_square_height_px;

		constexpr u8 g_screen_width_in_blocks = g_screen_width / g_block_width_px;
		constexpr u8 g_screen_height_in_blocks = g_screen_height / g_block_height_px;
	}
}