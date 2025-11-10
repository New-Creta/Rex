#include "rex_engine/gfx/system/resource_manager.h"

#include "rex_engine/diagnostics/log.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogResourceManager);

		void ResourceManager::add_render_target(RenderTargetBase* renderTarget, rsl::string_view name)
		{
			if (m_render_targets.contains(name))
			{
				REX_INFO(LogResourceManager, "Resource manager already contains a render target with name \"{}\". Cannot add another one with the same name");
				return;
			}

			m_render_targets.emplace(name, renderTarget);
		}
		void ResourceManager::add_depth_stencil_buffer(DepthStencilBuffer* depthStencilBuffer, rsl::string_view name)
		{
			if (m_depth_stencil_targets.contains(name))
			{
				REX_INFO(LogResourceManager, "Resource manager already contains a depth stencil buffer with name \"{}\". Cannot add another one with the same name");
				return;
			}

			m_depth_stencil_targets.emplace(name, depthStencilBuffer);
		}

		RenderTargetBase* ResourceManager::find_render_target(rsl::string_view name)
		{
			if (m_render_targets.contains(name))
			{
				return m_render_targets.at(name);
			}

			return nullptr;
		}
		DepthStencilBuffer* ResourceManager::find_depth_stencil_buffer(rsl::string_view name)
		{
			if (m_depth_stencil_targets.contains(name))
			{
				return m_depth_stencil_targets.at(name);
			}

			return nullptr;
		}

		namespace resource_manager
		{
			globals::GlobalUniquePtr<ResourceManager> g_resource_manager;
			void init(globals::GlobalUniquePtr<ResourceManager> resourceManager)
			{
				g_resource_manager = rsl::move(resourceManager);
			}
			ResourceManager* instance()
			{
				return g_resource_manager.get();
			}
			void shutdown()
			{
				g_resource_manager.reset();
			}
		}
	}
}