#include "pokemon/tileset.h"

#include "rex_engine/gfx/system/gal.h"
#include "rex_engine/engine/object_pool.h"

namespace pokemon
{
	TileSet::TileSet(const u8* tilesetData)
		: m_tileset_data(tilesetData)
	{}

	const u8* TileSet::tile_pixel_row(u8 tileIdx, s8 pxRow) const
	{
		const u8* tile_data_start = tile_data(tileIdx);
		
		return tile_data_start + (pxRow * s_tileset_width_px);
	}
	const u8* TileSet::tile_data(u8 tileIdx) const
	{
		constexpr s8 num_tiles_per_row = static_cast<s8>(s_tileset_width_px / Tile::width_px());;

		s32 tx = tileIdx % num_tiles_per_row;
		s32 ty = tileIdx / num_tiles_per_row;

		s32 tile_offset = Tile::height_px() * ty * (s_tileset_width_px * s_tile_pixel_byte_size) + (tx * Tile::width_px());

		return m_tileset_data + tile_offset;
	}


	//rsl::shared_ptr<TileSet> load_tileset(rsl::string_view filepath)
	//{
	//	// A tileset in OG pokemon was stored in 2bpp format.
	//	// However, storing them as PNGs with 2bit depth actually reduces them in size on disk due to compression
	//	// Therefore we don't need to do any building of of the bytes for a pixel as we can just them from the PNG
	//	s32 width, height, channels;
	//	u8* png_data = stbi_load(filepath.data(), &width, &height, &channels, 0);

	//	// We do have to convert the single byte that's loaded into a proper format for the GPU
	//	// As the value of a pixel is the value shared amongst all color components
	//	// with a fully opaque alpha channel, this is easy enough
	//	s32 num_pixels = width * height;
	//	rsl::vector<rsl::Rgba> pixels;
	//	pixels.reserve(num_pixels);

	//	rsl::Rgba pixel{};
	//	// To avoid setting the following in every iteration, just set it here, it's always the same anyway
	//	pixel.alpha = 255;
	//	for (s32 i = 0; i < num_pixels; ++i)
	//	{
	//		pixel.red = png_data[i];
	//		pixel.green = png_data[i];
	//		pixel.blue = png_data[i];
	//		pixels.push_back(pixel);
	//	}

	//	return rex::load_object<TileSet>(rex::gfx::gal::create_texture2d(width, height, rex::gfx::TextureFormat::Unorm4, pixels.data()));
	//}
}