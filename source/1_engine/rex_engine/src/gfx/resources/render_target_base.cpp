#include "rex_engine/gfx/resources/render_target_base.h"

namespace rex
{
	namespace gfx
	{
		RenderTargetBase::RenderTargetBase(s32 width, s32 height)
			: m_width(width)
			, m_height(height)
		{}

		s32 RenderTargetBase::width() const
		{
			return m_width;
		}

		s32 RenderTargetBase::height() const
		{
			return m_height;
		}
	}
}