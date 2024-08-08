#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/string.h"

#include "rex_renderer_core/resources/resource.h"

namespace rex
{
	namespace gfx
	{
		struct BoundResourceReflection;
		struct CBufferReflDesc;

		struct DxShaderPipelineParameters2
		{
			rsl::vector<CD3DX12_ROOT_PARAMETER> root_parameters;
			rsl::vector<rsl::vector<D3D12_DESCRIPTOR_RANGE>> ranges;

			rsl::vector<D3D12_DESCRIPTOR_RANGE> material_ranges;
			rsl::vector<D3D12_DESCRIPTOR_RANGE> renderpass_ranges;
		};
	}
}