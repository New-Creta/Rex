#pragma once

#include "rex_renderer_core/resources/resource.h"
#include "rex_renderer_core/gfx/sampler_filtering.h"
#include "rex_renderer_core/gfx/texture_address_mode.h"
#include "rex_renderer_core/gfx/border_color.h"
#include "rex_renderer_core/gfx/comparison_func.h"
#include "rex_renderer_core/gfx/shader_visibility.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		struct SamplerDesc
		{
		SamplerFiltering filtering = SamplerFiltering::Anisotropic; // The sampler's filtering method
		TextureAddressMode address_mode_u = TextureAddressMode::Wrap; // The sampler's address mode for the u component
		TextureAddressMode address_mode_v = TextureAddressMode::Wrap; // The sampler's address mode for the v component
		TextureAddressMode address_mode_w = TextureAddressMode::Wrap; // The sampler's address mode for the w component
		f32 mip_lod_bias = 0.0f; // Offset from calculated mip level. meaning if the gpu thinks it should sample at lvl 3 but this var is set to 2, the texture will be sampled at lvl 5
		s32 max_anisotropy = 0; // clamping value if using anisotrpoic filtering. values are valid between 1 and 16
		ComparisonFunc comparison_func = ComparisonFunc::Always; // A function that compares sampled data against existing sampled data. 
		BorderColor border_color = BorderColor::TransparentBlack; // The border color to use if any of the address mode is using TextureAddressMode::Border
		f32 min_lod = 0.0f; // lower end of the mip level range to use
		f32 max_lod = 0.0f; // higher end of the mip level range to use
		s32 shader_register = 0; // the shader register of the sampler
		s32 register_space = 0; // the shader register space of the sampler
		ShaderVisibility shader_visibility = ShaderVisibility::All; // the shaders this sampler is visible in
		};

		// A sampler is an object that holds the settings how a texture is applied on a triangle
		class Sampler2D : public Resource
		{
		public:
			Sampler2D(ResourceView* view)
				: m_view(view)
			{}
			virtual ~Sampler2D() = default;

			// Return the resource view of a sampler
			ResourceView* resource_view()
			{
				return m_view;
			}

		private:
			ResourceView* m_view;
		};
	}
}