#include "pokemon/oam.h"

namespace pokemon
{
	Oam::Oam()
	{
		m_gpu_buffer = make_unique<UnorderedAccessBuffer>(constants::g_oam_size);
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