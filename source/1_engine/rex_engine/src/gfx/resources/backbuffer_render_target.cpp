#include "rex_engine/gfx/resources/backbuffer_render_target.h"

namespace rex
{
	namespace gfx
	{
		BackBufferRenderTarget::BackBufferRenderTarget(Swapchain* swapchain)
			: RenderTargetBase(swapchain->width(), swapchain->height())
			, m_swapchain(swapchain)
		{}

		const ResourceView* BackBufferRenderTarget::view() const
		{
			return m_swapchain->current_buffer()->view();
		}

		const rsl::Color4f& BackBufferRenderTarget::clear_color() const
		{
			return m_swapchain->current_buffer()->clear_color();
		}

		void* BackBufferRenderTarget::api_object()
		{
			return swapchain()->current_buffer()->api_object();
		}

		Swapchain* BackBufferRenderTarget::swapchain()
		{
			return m_swapchain;
		}

	}
}