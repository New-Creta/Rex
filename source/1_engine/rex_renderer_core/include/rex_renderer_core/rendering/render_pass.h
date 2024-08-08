#pragma once

#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/system/shader_parameters_store.h"

#include "rex_std/string.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		class Texture2D;
		class Sampler2D;

		// A drawlist holds data needed to draw a single element to the screen
		struct DrawList
		{
			VertexBuffer* vb;   // The vertex buffer of the object
			IndexBuffer* ib;	// The index buffer of the object
			ConstantBuffer* per_instance_cb; // The constant buffer holding per instance data
		};

		// A descriptor on how to initialize the render pass
		struct RenderPassDesc
		{
			rsl::string_view name;			// The name of the render pass, mainly for debugging purposes
			PipelineStateDesc pso_desc;		// The pso desc that'll be used to create the pso for this render pipeline
		};
		
		// A render pass acts like the "material" for the render pipeline
		// It holds a set of data that needs to be tied to the render pipeline
		// as well as the logic on how to use the render pipeline to draw
		// elements to the screen
		class RenderPass
		{
		public:
			RenderPass(const RenderPassDesc& desc);

			// Bind the resources of the render pass to a render context
			void bind_resources(RenderContext* ctx);
			
			// Set renderpass parameter to a new resource
			void set(rsl::string_view name, ConstantBuffer* constantBuffer);
			void set(rsl::string_view name, Texture2D* texture);
			void set(rsl::string_view name, Sampler2D* sampler);

			// Return the slot of a renderpass parameter
			s32 slot(rsl::string_view name) const;

		private:
			rsl::tiny_stack_string m_name;								// The name of the renderpass
			rsl::unique_ptr<ShaderParametersStore> m_parameters_store;	// The store holding all renderpass parameters

			rsl::unique_ptr<PipelineState> m_pso;						// The pipeline state of the renderpass
		};
	}
}