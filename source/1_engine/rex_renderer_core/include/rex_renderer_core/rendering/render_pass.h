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

		struct DrawList
		{
			VertexBuffer* vb;
			IndexBuffer* ib;
			ConstantBuffer* cb;
		};

		struct RenderPassDesc
		{
			rsl::string_view name;
			PipelineStateDesc pso_desc;
		};

		// A render pass holds a set of well defined parameters
		// It owns the pipeline state as well as the constant buffers that are used by it
		class RenderPass
		{
		public:
			RenderPass(const RenderPassDesc& desc);

			void bind_resources(RenderContext* ctx);
			
			void set(rsl::string_view name, ConstantBuffer* constantBuffer);
			void set(rsl::string_view name, Texture2D* texture);
			void set(rsl::string_view name, Sampler2D* sampler);

			s32 slot(rsl::string_view name) const;

		private:
			rsl::tiny_stack_string m_name;
			rsl::unique_ptr<ShaderParametersStore> m_parameters_store;

			rsl::unique_ptr<PipelineState> m_pso;
		};
	}
}