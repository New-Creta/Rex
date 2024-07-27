#pragma once

#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
	namespace gfx
	{
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

			ScopedPoolObject<RenderContext> begin();
			void end(ScopedPoolObject<RenderContext>&);

		private:
			rsl::tiny_stack_string m_name;

			rsl::unique_ptr<PipelineState> m_pso;
			rsl::unique_ptr<RootSignature> m_root_signature;
		};
	}
}