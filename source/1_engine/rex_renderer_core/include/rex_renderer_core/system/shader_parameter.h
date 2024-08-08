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

		// The parameter that's bound to the shader
		enum class ShaderParameterType
		{
			Undefined,
			ConstantBuffer,
			Texture,
			Sampler
		};

		// A descriptor describing the parameter that's bound to the shader
		struct ShaderParameterDesc
		{
			ShaderParameterType type;
			s32 slot;
			s32 num_views;
		};

		// Base class for shader parameters, holding common data that's shared between shader parameters
		class ShaderParameter
		{
		public:
			ShaderParameter(const ShaderParameterDesc& desc);
			virtual ~ShaderParameter() = default;

			// The slot of a parameter
			s32 slot() const;
			// The type of a parameter
			ShaderParameterType type() const;

			// Update a certain resource within the shader parameter (it's possible there's only 1)
			virtual void update_view(ViewOffset offset, ConstantBuffer* cb) = 0;
			virtual void update_view(ViewOffset offset, Texture2D* texture) = 0;
			virtual void update_view(ViewOffset offset, Sampler2D* sampler) = 0;

			// Bind the shader parameter to a render context
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

			// Update a certain resource within the shader parameter, guaranteed there's only 1
			void update_view(ViewOffset offset, ConstantBuffer* cb) override;
			void update_view(ViewOffset offset, Texture2D* texture) override;
			void update_view(ViewOffset offset, Sampler2D* sampler) override;

			// Bind the shader parameter to a render context
			void bind_to(RenderContext* ctx) const override;

		private:
			// Update a certain resource within the shader parameter, guaranteed there's only 1
			void update_view(ViewOffset offset, u64 gpuAddress);

		private:
			// For single views, we need to pass the gpu address
			// as in DirectX we know the type of the resource as the function describes it (eg SetGraphicsRootConstantBufferView)
			// therefore they don't accept a resource view but the address of the resource instead
			u64 m_gpu_address;
		};

		class ViewTableShaderParam : public ShaderParameter
		{
		public:
			ViewTableShaderParam(const ShaderParameterDesc& desc);

			// Update a certain resource within the shader parameter (it's possible there's only 1)
			void update_view(ViewOffset offset, ConstantBuffer* cb) override;
			void update_view(ViewOffset offset, Texture2D* texture) override;
			void update_view(ViewOffset offset, Sampler2D* sampler) override;

			// Bind the shader parameter to a render context
			void bind_to(RenderContext* ctx) const override;

		private:
			// Update a certain resource within the shader parameter (it's possible there's only 1)
			void update_view(ViewOffset offset, ResourceView* view);

		private:
			rsl::vector<ResourceView*> m_views;
		};

	}
}