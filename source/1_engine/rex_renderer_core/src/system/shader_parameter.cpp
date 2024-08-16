#include "rex_renderer_core/system/shader_parameter.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/resources/sampler_2d.h"

#include "rex_renderer_core/gfx/render_context.h"
#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
	namespace gfx
	{
		// --------------------------
		// Shader Parameter
		// --------------------------
		ShaderParameter::ShaderParameter(const ShaderParameterDesc& desc)
			: m_slot(desc.slot)
			, m_type(desc.type)
		{}
		s32 ShaderParameter::slot() const
		{
			return m_slot;
		}
		ShaderParameterType ShaderParameter::type() const
		{
			return m_type;
		}

		// --------------------------
		// View Shader Parameter
		// --------------------------
		ViewShaderParam::ViewShaderParam(const ShaderParameterDesc& desc)
			: ShaderParameter(desc)
			, m_gpu_address(0)
		{}
		void ViewShaderParam::update_view(ViewOffset offset, ConstantBuffer* cb)
		{
			update_view(offset, cb->gpu_address());
		}
		void ViewShaderParam::update_view(ViewOffset, Texture2D*)
		{
			REX_ASSERT("Textures cannot be tied to an inline view. They need to be tied to a view table");
		}
		void ViewShaderParam::update_view(ViewOffset, Sampler2D*)
		{
			REX_ASSERT("Samplers cannot be tied to an inline view. They need to be tied to a view table");
		}
		void ViewShaderParam::bind_to(RenderContext* ctx) const
		{
			switch (type())
			{
			case ShaderParameterType::ConstantBuffer: ctx->bind_constant_buffer(slot(), m_gpu_address); break;
			default: REX_ASSERT("Invalid shader resource type request to bind");
			}
		}
		void ViewShaderParam::update_view(ViewOffset offset, u64 gpuAddress)
		{
			REX_ASSERT_X(offset.range_offset == 0, "Invalid range offset for binding a single view");
			REX_ASSERT_X(offset.offset_within_range == 0, "Invalid offset within range for binding a single view");
			m_gpu_address = gpuAddress;
		}

		// --------------------------
		// View Table Shader Parameter
		// --------------------------
		ViewTableShaderParam::ViewTableShaderParam(const ShaderParameterDesc& desc)
			: ShaderParameter(desc)
		{
			m_views.resize(desc.num_views);
		}
		void ViewTableShaderParam::update_view(ViewOffset offset, ConstantBuffer* cb)
		{
			update_view(offset, cb->resource_view());
		}
		void ViewTableShaderParam::update_view(ViewOffset offset, Texture2D* texture)
		{
			update_view(offset, texture->resource_view());
		}
		void ViewTableShaderParam::update_view(ViewOffset offset, Sampler2D* sampler)
		{
			update_view(offset, sampler->resource_view());
		}
		void ViewTableShaderParam::bind_to(RenderContext* ctx) const
		{
			ViewHeapType target_view_heap_type;
			switch (type())
			{
			case ShaderParameterType::ConstantBuffer: target_view_heap_type = ViewHeapType::ConstantBuffer; break;
			case ShaderParameterType::Texture: target_view_heap_type = ViewHeapType::Texture2D; break;
			case ShaderParameterType::Sampler: target_view_heap_type = ViewHeapType::Sampler; break;
			default: REX_ASSERT("Invalid shader parameter type"); break;
			}

			auto copy_ctx = new_copy_ctx();
			auto start_handle = copy_ctx->copy_views(target_view_heap_type, m_views);
			ctx->bind_view_table(slot(), start_handle.get());
		}

		void ViewTableShaderParam::update_view(ViewOffset offset, ResourceView* view)
		{
			s32 abs_offset = offset.range_offset * offset.offset_within_range;
			m_views[abs_offset] = view;
		}
	}
}