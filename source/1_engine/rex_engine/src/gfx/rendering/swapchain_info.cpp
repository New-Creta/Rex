#include "rex_engine/gfx/rendering/swapchain_info.h"

namespace rex
{
	namespace gfx
	{
		SwapchainInfo g_swapchain_info;

		const SwapchainInfo& swapchain_info()
		{
			return g_swapchain_info;
		}
		void update_swapchain_info(const SwapchainInfo& swapchainInfo)
		{
			g_swapchain_info = swapchainInfo;
		}

		SwapchainFrameBufferHandle swapchain_frame_buffer_handle()
		{
			return SwapchainFrameBufferHandle{};
		}
	}
}