#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/system/view_param.h"

namespace rex
{
	namespace gfx
	{
		class ConstantBuffer;
		class Texture2D;
		class Sampler2D;
		class ResourceView;
		class RenderContext;

		// The resource that's bound to the shader
		enum class ShaderParameterType
		{
			Undefined,
			ConstantBuffer,
			Texture,
			Sampler
		};

		struct ShaderParameterDesc
		{
			ShaderParameterType type;
			s32 slot;
			s32 num_views;
		};

		class ShaderParameter
		{
		public:
			ShaderParameter(const ShaderParameterDesc& desc);
			virtual ~ShaderParameter() = default;

			s32 slot() const;
			ShaderParameterType type() const;

			virtual void update_view(ViewOffset offset, ConstantBuffer* cb) = 0;
			virtual void update_view(ViewOffset offset, Texture2D* texture) = 0;
			virtual void update_view(ViewOffset offset, Sampler2D* sampler) = 0;
			virtual void bind_to(RenderContext* ctx) const = 0;

		private:
			rsl::vector<ResourceView*> m_views;
			ShaderParameterType m_type;
			s32 m_slot;
		};

		class ViewShaderParam : public ShaderParameter
		{
		public:
			ViewShaderParam(const ShaderParameterDesc& desc);

			void update_view(ViewOffset offset, ConstantBuffer* cb) override;
			void update_view(ViewOffset offset, Texture2D* texture) override;
			void update_view(ViewOffset offset, Sampler2D* sampler) override;
			void bind_to(RenderContext* ctx) const override;

		private:
			void update_view(ViewOffset offset, u64 gpuAddress);

		private:
			u64 m_gpu_address;
		};

		class ViewTableShaderParam : public ShaderParameter
		{
		public:
			ViewTableShaderParam(const ShaderParameterDesc& desc);

			void update_view(ViewOffset offset, ConstantBuffer* cb) override;
			void update_view(ViewOffset offset, Texture2D* texture) override;
			void update_view(ViewOffset offset, Sampler2D* sampler) override;
			void bind_to(RenderContext* ctx) const override;

		private:
			void update_view(ViewOffset offset, ResourceView* view);

		private:
			rsl::vector<ResourceView*> m_views;
		};

	}
}