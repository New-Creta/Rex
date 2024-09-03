#include "rex_engine/gfx/resources/clear_state.h"

namespace rex
{
	namespace gfx
	{
		ClearStateDesc default_rtv_clear_state()
		{
			ClearStateDesc desc{};
			desc.flags.add_state(ClearBits::ClearColorBuffer);
			desc.rgba = rsl::colors::Cyan;

			return desc;
		}
		ClearStateDesc default_depth_clear_state()
		{
			ClearStateDesc desc{};
			desc.flags.add_state(ClearBits::ClearDepthBuffer);
			desc.depth = 1.0f;
			return desc;
		}
		ClearStateDesc default_stencil_clear_state()
		{
			ClearStateDesc desc{};
			desc.flags.add_state(ClearBits::ClearStencilBuffer);
			desc.stencil = 0;
			return desc;
		}
		ClearStateDesc default_dsv_clear_state()
		{
			ClearStateDesc desc{};
			desc.flags.add_state(ClearBits::ClearDepthBuffer);
			desc.flags.add_state(ClearBits::ClearStencilBuffer);
			desc.depth = 1.0f;
			desc.stencil = 0;
			return desc;
		}
	}
}