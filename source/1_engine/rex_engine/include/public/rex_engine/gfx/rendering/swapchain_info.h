#pragma once

#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/core/viewport.h"
#include "rex_engine/gfx/core/scissor_rect.h"

namespace rex
{
	namespace gfx
	{
		class RenderTarget;

		struct SwapchainInfo
		{
			s32 width;
			s32 height;

			Viewport viewport;				// A viewport spanning the back buffer's resolution
			ScissorRect scissor_rect;	// A scissor rect spanning the back buffer's resolution
		};

		const SwapchainInfo& swapchain_info();
		void update_swapchain_info(const SwapchainInfo& swapchainInfo);

		// An empty structure, it's just to indicate that the swapchain's backbuffer should be used as color buffer
		// We can't save the backbuffer's render target as a member of the swapchain info as it updates every frame
		// When render passes want to use it as a render target, they need some form of indirection to this render target
		// Therefore they can use the below handle
		struct SwapchainFrameBufferHandle {};
		// This function returns a new swapchain frame buffer handle
		SwapchainFrameBufferHandle swapchain_frame_buffer_handle();
	}
}