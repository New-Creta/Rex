#include "rex_engine/gfx/resources/depth_stencil_buffer.h"

namespace rex
{
	namespace gfx
	{
		DepthStencilBuffer::DepthStencilBuffer(ResourceView* view, s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
			: m_width(width)
			, m_height(height)
			, m_format(format)
			, m_view(view)
			, m_clear_state(clearStateDesc)
		{

		}

		s32 DepthStencilBuffer::width() const
		{
			return m_width;
		}
		s32 DepthStencilBuffer::height() const
		{
			return m_height;
		}
		TextureFormat DepthStencilBuffer::format() const
		{
			return m_format;
		}
		ResourceView* DepthStencilBuffer::resource_view()
		{
			return m_view;
		}
		const ClearStateDesc& DepthStencilBuffer::clear_state() const
		{
			return m_clear_state;
		}

	}
}