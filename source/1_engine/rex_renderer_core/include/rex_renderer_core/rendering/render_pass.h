#pragma once

#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/gfx/graphics.h"

#include "rex_std/string.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		struct RenderPassDesc
		{
			rsl::string_view name;
			PipelineStateDesc pso_desc;
		};

		enum class ShdrParamType
		{
			ConstantBuffer
		};
		class ShaderParameter3
		{
		public:
			ShaderParameter3(s32 slot, ShdrParamType type)
				: m_slot(slot)
				, m_resource(nullptr)
				, m_type(type)
			{
			}

			void set(Resource* resource)
			{
				m_resource = resource;
			}

			Resource* resource()
			{
				return m_resource;
			}

			s32 slot() const
			{
				return m_slot;
			}

		private:
			s32 m_slot;
			Resource* m_resource;
			ShdrParamType m_type;
		};

		// A render pass holds a set of well defined parameters
		// It owns the pipeline state as well as the constant buffers that are used by it
		class RenderPass
		{
		public:
			RenderPass(const RenderPassDesc& desc);

			ScopedPoolObject<RenderContext> bind_resources();

			void set(rsl::string_view key, ConstantBuffer* constantBuffer);

			s32 slot(rsl::string_view key) const;

		private:
			rsl::tiny_stack_string m_name;

			rsl::unique_ptr<PipelineState> m_pso;
			rsl::unique_ptr<RootSignature> m_root_signature;

			rsl::unordered_map<rsl::string, ShaderParameter3> m_shader_parameters;
		};
	}
}