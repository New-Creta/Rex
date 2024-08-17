#pragma once

#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/system/shader_parameters_store.h"
#include "rex_renderer_core/resources/depth_stencil_buffer.h"
#include "rex_renderer_core/resources/clear_state.h"

#include "rex_engine/text_processing/json.h"

#include "rex_std/string.h"
#include "rex_std/memory.h"
#include "rex_std/optional.h"
#include "rex_std/unordered_map.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/rendering/frame_buffer.h"

namespace rex
{
	namespace gfx
	{
		class Texture2D;
		class Sampler2D;
		
		// A descriptor on how to initialize the render pass
		struct RenderPassDesc
		{
			rsl::string_view name;			// The name of the render pass, mainly for debugging purposes
			PipelineStateDesc pso_desc;		// The pso desc that'll be used to create the pso for this render pass
			FrameBufferDesc framebuffer_desc; // The framebuffer desc that'll be used to create the framebuffer for this render pass
		};

		struct MaterialPipelineState
		{
			rsl::unique_ptr<ShaderParametersStore> parameter_store;
			rsl::unique_ptr<PipelineState> pso;
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
			void bind_to(RenderContext* ctx);
			// Bind a material to the renderpass, possibly overwriting the PSO of the render pass
			void bind_material(RenderContext* ctx, Material* material);
			
			// Set renderpass parameter to a new resource
			void set(rsl::string_view name, ConstantBuffer* constantBuffer);
			void set(rsl::string_view name, Texture2D* texture);
			void set(rsl::string_view name, Sampler2D* sampler);

			// Return the slot of a renderpass parameter
			s32 slot(rsl::string_view name) const;

			// Set the blend factor of the material
			void set_blend_factor(const BlendFactor& blendFactor);

		private:
			void bind_params_to_pipeline(ShaderParametersStore* paramStore, RenderContext* ctx);

		private:
			rsl::tiny_stack_string m_name;															// The name of the renderpass
			rsl::unique_ptr<ShaderParametersStore> m_parameters_store;	// The store holding all renderpass parameters
			rsl::unique_ptr<FrameBuffer> m_framebuffer;									// The framebuffer used by the render pass

			BlendFactor m_blend_factor;																	// The blend factor used by the render pass

			rsl::unique_ptr<PipelineState> m_pso;												// The pipeline state of the renderpass
			PipelineStateDesc m_pso_desc;																// The description of the PSO used for the render pass
			rsl::unordered_map<Material*, MaterialPipelineState> m_material_to_pipeline_state;
		};
	}
}