#pragma once

#include "rex_engine/engine/globals.h"

#include "rex_engine/gfx/resources/resource.h"
#include "rex_engine/gfx/system/resource_view.h"

#include "rex_std/unordered_map.h"

namespace rex
{
	namespace gfx
	{
		class RenderTargetBase;
		class DepthStencilBuffer;

		// The resource manager is responsible for tracking all GPU resources
		// This is to avoid multiple of the same resources to be pushed to the GPU
		class ResourceManager
		{
		public:
			// As not every render target needs to be accessible in the shader, we don't store resource views inside render targets
			// but store them in the resource manager instead
			ResourceView* store_shader_resource_view(const RenderTargetBase* rt, rsl::unique_ptr<ResourceView> view);
			ResourceView* shader_resource_view(const RenderTargetBase* rt);

			void add_render_target(RenderTargetBase* renderTarget, rsl::string_view name);
			void add_depth_stencil_buffer(DepthStencilBuffer* depthStencilBuffer, rsl::string_view name);

			RenderTargetBase* find_render_target(rsl::string_view name);
			DepthStencilBuffer* find_depth_stencil_buffer(rsl::string_view name);

		private:
			rsl::unordered_map<const RenderTargetBase*, rsl::unique_ptr<ResourceView>> m_render_targets_resource_views;

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