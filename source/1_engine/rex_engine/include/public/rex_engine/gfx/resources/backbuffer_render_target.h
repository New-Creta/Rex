#pragma once

#include "rex_engine/gfx/resources/render_target_base.h"

#include "rex_engine/gfx/system/swapchain.h"

namespace rex
{
	namespace gfx
	{
		// A class that represents the current active backbuffer as a render target
		class BackBufferRenderTarget : public RenderTargetBase
		{
		public:
			BackBufferRenderTarget(Swapchain* swapchain);

			const ResourceView* view() const override;
			const rsl::Color4f& clear_color() const override;

			void* api_object() const override;

		protected:
			Swapchain* swapchain();

		private:
			Swapchain* m_swapchain;
		};
	}
}