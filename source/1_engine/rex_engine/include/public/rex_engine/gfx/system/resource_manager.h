#pragma once

#include "rex_engine/engine/globals.h"

#include "rex_engine/gfx/resources/resource.h"

#include "rex_std/unordered_map.h"

namespace rex
{
	namespace gfx
	{
		class RenderTargetBase;
		class DepthStencilBuffer;

		class ResourceManager
		{
		public:
			void add_render_target(RenderTargetBase* renderTarget, rsl::string_view name);
			void add_depth_stencil_buffer(DepthStencilBuffer* depthStencilBuffer, rsl::string_view name);

			RenderTargetBase* find_render_target(rsl::string_view name);
			DepthStencilBuffer* find_depth_stencil_buffer(rsl::string_view name);

		private:
			rsl::unordered_map<rsl::string_view, RenderTargetBase*> m_render_targets;
			rsl::unordered_map<rsl::string_view, DepthStencilBuffer*> m_depth_stencil_targets;
		};
		
		namespace resource_manager
		{
			void init(globals::GlobalUniquePtr<ResourceManager> resourceManager);
			ResourceManager* instance();
			void shutdown();
		}
	}
}