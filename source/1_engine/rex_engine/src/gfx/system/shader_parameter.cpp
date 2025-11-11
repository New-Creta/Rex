#include "rex_engine/gfx/system/shader_parameter.h"

#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/sampler_2d.h"

#include "rex_engine/gfx/system/render_context.h"
#include "rex_engine/gfx/graphics.h"

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
		void ViewShaderParam::update_view(ViewOffset offset, const ConstantBuffer* cb)
		{
			update_view(offset, cb->gpu_address());
		}
		void ViewShaderParam::update_view(ViewOffset offset, const UnorderedAccessBuffer* uab)
		{
			update_view(offset, uab->gpu_address());
		}
		void ViewShaderParam::update_view(ViewOffset, const Texture2D*)
		{
			REX_ASSERT("Textures cannot be tied to an inline view. They need to be tied to a view table");
		}
		void ViewShaderParam::update_view(ViewOffset, const Sampler2D*)
		{
			REX_ASSERT("Samplers cannot be tied to an inline view. They need to be tied to a view table");
		}
		void ViewShaderParam::update_view(ViewOffset offset, const RenderTarget* rt)
		{
			REX_ASSERT("Render targets (as textures) cannot be tied to an inline view. They need to be tied to a view table");
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
			REX_UNUSED_PARAM(offset);

			REX_ASSERT_X(offset.abs_offset == 0, "Invalid range offset for binding a single view");
			//REX_ASSERT_X(offset.offset_within_range == 0, "Invalid offset within range for binding a single view");
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
		void ViewTableShaderParam::update_view(ViewOffset offset, const ConstantBuffer* cb)
		{
			update_view(offset, cb->resource_view());
		}
		void ViewTableShaderParam::update_view(ViewOffset offset, const UnorderedAccessBuffer* uab)
		{
			update_view(offset, uab->resource_view());
		}
		void ViewTableShaderParam::update_view(ViewOffset offset, const Texture2D* texture)
		{
			update_view(offset, texture->resource_view());
		}
		void ViewTableShaderParam::update_view(ViewOffset offset, const Sampler2D* sampler)
		{
			update_view(offset, sampler->resource_view());
		}
		void ViewTableShaderParam::update_view(ViewOffset offset, const RenderTarget* rt)
		{
			const ResourceView* view = gal::instance()->create_srv(rt);
			update_view(offset, view);
		}
		void ViewTableShaderParam::bind_to(RenderContext* ctx) const
		{
			ResourceViewType target_view_heap_type = ResourceViewType::Undefined;
			switch (type())
			{
			case ShaderParameterType::ByteAddress: target_view_heap_type = ResourceViewType::ByteAddress; break;
			case ShaderParameterType::UnorderedAccessView: target_view_heap_type = ResourceViewType::UnorderedAccess; break;
			case ShaderParameterType::ConstantBuffer: target_view_heap_type = ResourceViewType::ConstantBuffer; break;
			case ShaderParameterType::Texture: target_view_heap_type = ResourceViewType::Texture2D; break;
			case ShaderParameterType::Sampler: target_view_heap_type = ResourceViewType::Sampler; break;
			default: REX_ASSERT("Invalid shader parameter type"); break;
			}

			auto start_handle = ctx->copy_views(target_view_heap_type, m_views);
			ctx->bind_view_table(slot(), start_handle);
		}

		void ViewTableShaderParam::update_view(ViewOffset offset, const ResourceView* view)
		{
			m_views[offset.abs_offset] = view;
		}
	}
}