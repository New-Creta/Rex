#include "pokemon/oam.h"

#include "pokemon/render_constants.h"

namespace pokemon
{
	rsl::pointi8 calc_oam_coord(s8 x_tile, s8 y_tile, s8 x_pixel, s8 y_pixel)
	{
		rsl::pointi8 coord{};
		coord.x = (y_tile * constants::g_tile_width_px) % 0x100 + y_pixel;
		coord.y = (x_tile * constants::g_tile_height_px) % 0x100 + x_pixel;
		return coord;
	}

	Oam::Oam()
	{
		m_gpu_buffer = make_unique<UnorderedAccessBuffer>(constants::g_oam_size);
	}

	void Oam::set(s32 idx, OAMStruct oamData)
	{
		m_objects[idx] = oamData;
	}

	namespace oam
	{
		rex::globals::GlobalUniquePtr<Oam> g_oam;

		void init(rex::globals::GlobalUniquePtr<Oam>&& oam)
		{
			g_oam = rsl::move(oam);
		}
		Oam* instance()
		{
			return g_oam.get();
		}
		void shutdown()
		{
			g_oam.reset();
		}
	}
}