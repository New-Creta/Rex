#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		class ShaderPipelineReflection;

		class DxShaderRootParameters
		{
		public:
			DxShaderRootParameters(const ShaderSignature* signature, ShaderVisibility shaderVis);

			const rsl::vector<CD3DX12_ROOT_PARAMETER>& params() const;
			s32 count() const;

		private:
			void add_to_view_range(rsl::vector<D3D12_DESCRIPTOR_RANGE>& ranges, s32 startRegister, s32 lastRegister, D3D12_DESCRIPTOR_RANGE_TYPE type);

		private:
			rsl::vector<CD3DX12_ROOT_PARAMETER> m_root_parameters;

			// The descriptor ranges need to stick around as they don't get copied into the root signature directly
			// Instead a pointer to them is given, so if we'd allocate them locally, they'd be destroyed by the time they're used
			rsl::vector<D3D12_DESCRIPTOR_RANGE> m_texture_ranges;
			rsl::vector<D3D12_DESCRIPTOR_RANGE> m_sampler_ranges;

		};
	}
}