#include "rex_engine/gfx/system/gal.h"

namespace rex
{
	namespace gfx
	{
    rsl::unique_ptr<GALInterface> g_gal;

    GALInterface* gal()
    {
      return g_gal.get();
    }

    // Initializes the render hardware infrastructure
		// For DirectX, creates the dxgi factory, d3d device, command buffers, heaps and swapchain
		// After this, the gal is setup to start creating resources (textures, shaders, vertex buffers, ..)
		gfx::GpuEngine* init_gal(rsl::unique_ptr<GALInterface> galInterface, const OutputWindowUserData& userData)
		{
			REX_ASSERT_X(g_gal == nullptr, "The graphics abstraction layer has already been assigned, this is not allowed");
			g_gal = rsl::move(galInterface);
			return g_gal->create_gpu_engine(userData);
		}

		// shutdown the internal gal, all reference to the gal are invalidated from here on out
		void shutdown_gal()
		{
			g_gal.reset();
		}
	}
}