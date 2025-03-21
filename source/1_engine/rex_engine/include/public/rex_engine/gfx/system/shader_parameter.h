#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/system/shader_parameter_location.h"

namespace rex
{
	namespace gfx
	{
		class ConstantBuffer;
		class UnorderedAccessBuffer;
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
			Sampler,
			ByteAddress,
			UnorderedAccessView
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
			virtual void update_view(ViewOffset offset, const ConstantBuffer* cb) = 0;
			virtual void update_view(ViewOffset offset, const UnorderedAccessBuffer* uab) = 0;
			virtual void update_view(ViewOffset offset, const Texture2D* texture) = 0;
			virtual void update_view(ViewOffset offset, const Sampler2D* sampler) = 0;

			// Bind the shader parameter to a render context
			virtual void bind_to(RenderContext* ctx) const = 0;

		private:
			ShaderParameterType m_type;
			s32 m_slot;
		};

		class ViewShaderParam : public ShaderParameter
		{
		public:
			ViewShaderParam(const ShaderParameterDesc& desc);

			// Update a certain resource within the shader parameter, guaranteed there's only 1
			void update_view(ViewOffset offset, const ConstantBuffer* cb) override;
			void update_view(ViewOffset offset, const UnorderedAccessBuffer* uab) override;
			void update_view(ViewOffset offset, const Texture2D* texture) override;
			void update_view(ViewOffset offset, const Sampler2D* sampler) override;

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
			void update_view(ViewOffset offset, const ConstantBuffer* cb) override;
			void update_view(ViewOffset offset, const UnorderedAccessBuffer* uab) override;
			void update_view(ViewOffset offset, const Texture2D* texture) override;
			void update_view(ViewOffset offset, const Sampler2D* sampler) override;

			// Bind the shader parameter to a render context
			void bind_to(RenderContext* ctx) const override;

		private:
			// Update a certain resource within the shader parameter (it's possible there's only 1)
			void update_view(ViewOffset offset, const ResourceView* view);

		private:
			rsl::vector<const ResourceView*> m_views;
		};

	}
}