#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string_view>
#include <bitset>
#include <sstream>
#include <string>

std::vector<char> load_file(std::string_view filepath)
{
	std::fstream f(filepath.data(), std::ios::in | std::ios::ate | std::ios::binary);
	if (!f.is_open())
	{
		return {};
	}

	auto size = f.tellg();
	std::vector<char> content(size);
	f.seekg(0);
	f.read(content.data(), size);
	return content;
}

char tileset_pixel_value(int idx, char evenByte, char oddByte)
{
	char even_value = (evenByte >> (7 - idx)) & 0x1;
	char odd_value = (oddByte >> (7 - idx)) & 0x1;
	odd_value <<= 1;

	return even_value | odd_value;
}

struct tile
{
	std::vector<char> pixels;
};
std::vector<tile> read_tiles(const std::vector<char>& tileset)
{
	// Tiles are 8x8 pixels, read from top left to bottom right
	// all the high bits are in the first byte, all the low bits are in the second byte.
	// A single tile is represented by 2 bytes
	std::vector<tile> tiles;
	int tile_width_in_bytes = 16;
	int num_tiles = tileset.size() / tile_width_in_bytes;
	for (int i = 0; i < tileset.size(); i += tile_width_in_bytes)
	{
		int tile_idx = tiles.size();
		tile& tile = tiles.emplace_back();
		for (int j = 0; j < 16; j += 2)
		{
			int tile_byte_start = (tile_idx * tile_width_in_bytes);
			char even_byte = tileset[tile_byte_start + j + 0];
			char odd_byte = tileset[tile_byte_start + j + 1];

			// 1 row at a time
			tile.pixels.push_back(tileset_pixel_value(0, even_byte, odd_byte));
			tile.pixels.push_back(tileset_pixel_value(1, even_byte, odd_byte));
			tile.pixels.push_back(tileset_pixel_value(2, even_byte, odd_byte));
			tile.pixels.push_back(tileset_pixel_value(3, even_byte, odd_byte));
			tile.pixels.push_back(tileset_pixel_value(4, even_byte, odd_byte));
			tile.pixels.push_back(tileset_pixel_value(5, even_byte, odd_byte));
			tile.pixels.push_back(tileset_pixel_value(6, even_byte, odd_byte));
			tile.pixels.push_back(tileset_pixel_value(7, even_byte, odd_byte));
		}
	}

	return tiles;
}

struct block
{
	std::vector<char> indices;
};
std::vector<block> read_blocks(const std::vector<char>& blockSet)
{
	std::vector<block> blocks;
	for (int i = 0; i < blockSet.size(); i += 16)
	{
		block& block = blocks.emplace_back();
		block.indices.push_back(blockSet[i + 0x0]);
		block.indices.push_back(blockSet[i + 0x1]);
		block.indices.push_back(blockSet[i + 0x2]);
		block.indices.push_back(blockSet[i + 0x3]);
		block.indices.push_back(blockSet[i + 0x4]);
		block.indices.push_back(blockSet[i + 0x5]);
		block.indices.push_back(blockSet[i + 0x6]);
		block.indices.push_back(blockSet[i + 0x7]);
		block.indices.push_back(blockSet[i + 0x8]);
		block.indices.push_back(blockSet[i + 0x9]);
		block.indices.push_back(blockSet[i + 0xA]);
		block.indices.push_back(blockSet[i + 0xB]);
		block.indices.push_back(blockSet[i + 0xC]);
		block.indices.push_back(blockSet[i + 0xD]);
		block.indices.push_back(blockSet[i + 0xE]);
		block.indices.push_back(blockSet[i + 0xF]);
	}
	return blocks;
}

#include <Windows.h>
void set_console_pos(int x, int y)
{
	static HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord{};
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(output_handle, coord);
}

std::vector<std::string> lines;

void draw_tile(int startX, int startY, const tile& tile)
{
	static std::stringstream ss;
	ss.str("");

	// a tile print 8 rows of 8 pixels per row
	int width_px = 8;
	int num_rows = 8;
	int chars_per_pixel = 3;
	for (int row = 0; row < num_rows; ++row)
	{
		int pixel_x = startX;
		int pixel_y = startY + row;
		set_console_pos(pixel_x, pixel_y);
		for (int pixel_idx = 0; pixel_idx < width_px; ++pixel_idx)
		{
			int abs_pixel_idx = (row * width_px) + pixel_idx;
			char pixel = tile.pixels[abs_pixel_idx];
			std::bitset<2> bitset(pixel);
			std::cout << bitset << " ";
			ss << bitset << " ";
			lines[pixel_y] += ss.str();
			ss.str("");
		}
	}
}

void draw_block(int startX, int startY, const std::vector<tile>& tiles, const block& block)
{
	// a block print 4 rows of each 4 tiles
	int num_tiles_per_row = 4;
	int num_rows = 4;
	int chars_per_pixel = 3;
	int width_px = 8;
	int height_px = 8;

	for (int row = 0; row < num_rows; ++row)
	{
		for (int tile_idx = 0; tile_idx < num_tiles_per_row; ++tile_idx)
		{
			int tile_top_left_x = startX + tile_idx * width_px * chars_per_pixel;
			int tile_top_left_y = startY + row * height_px;
			int abs_idx_in_block_indices = (row * num_tiles_per_row) + tile_idx;
			int block_tile_idx = block.indices[abs_idx_in_block_indices];
			const tile& tile = tiles[block_tile_idx];
			draw_tile(tile_top_left_x, tile_top_left_y, tile);
		}
	}
}

int main()
{
	const char white = '⬜';
	const char light_gray = '🔳';
	const char dark_gray = '🔲';
	const char black = '⬛';

	std::unordered_map<char, char> pixel_to_char
	{
		{0, white},
		{1, light_gray},
		{2, dark_gray},
		{3, black}
	};

	std::vector<char> tileset = load_file("overworld.2bpp");
	std::vector<char> blockset = load_file("overworld.bst");
	std::vector<char> map = load_file("pallettown.blk");

	std::vector<tile> tiles = read_tiles(tileset);
	std::vector<block> blocks = read_blocks(blockset);

	std::cout.setf(std::ios_base::binary, std::ios_base::basefield);


	int width = 10;
	int height = 9;
	int tile_width_px = 8;
	int tile_height_px = 8;
	int block_width_px = tile_width_px * 4;
	int block_height_px = tile_height_px * 4;
	int chars_per_pixel = 3;

	lines.resize(height * block_height_px);

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			int block_idx_in_map = (i * width) + j;
			int block_idx = map[block_idx_in_map];
			const block& block = blocks[block_idx];
			int x_offset = j * (block_width_px * chars_per_pixel);
			int y_offset = i * block_height_px;
			draw_block(x_offset, y_offset, tiles, block);
		}
	}

	std::fstream output("pallet_town_output.txt", std::ios::out);
	for (std::string_view line : lines)
	{
		output << line << "\n";
	}
}