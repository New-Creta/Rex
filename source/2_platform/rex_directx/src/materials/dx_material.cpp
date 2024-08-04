#include "rex_directx/materials/dx_material.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_directx/resources/dx_sampler_2d.h"

namespace rex
{
	namespace gfx
	{
		//void DxMaterial::set(rsl::string_view name, ConstantBuffer* constantBuffer)
		//{
		//	DxConstantBuffer* dx_cb = d3d::to_dx12(constantBuffer);
		//	s32 idx = index_of_param(name);
		//	m_constant_buffers[idx] = constantBuffer;
		//}
		//void DxMaterial::set(rsl::string_view name, Texture2D* texture)
		//{
		//	DxTexture2D* dx_texture = d3d::to_dx12(texture);
		//	s32 idx = index_of_param(name);
		//	m_textures[idx] = texture;
		//}
		//void DxMaterial::set(rsl::string_view name, Sampler2D* sampler)
		//{
		//	DxSampler2D* dx_sampler = d3d::to_dx12(sampler);
		//	s32 idx = index_of_param(name);
		//	m_samplers[idx] = sampler;
		//}
	}
}