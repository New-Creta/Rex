#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/resources/resource.h"

namespace rex
{
	namespace gfx
	{
		struct ShaderResourceDeclaration;
		struct ConstantBufferDeclaration;

		struct DxShaderPipelineParameters
		{
			// The root parameters that will be used to initialize the root signature			
			rsl::vector<CD3DX12_ROOT_PARAMETER> root_parameters;
			// As the root parameters could contain descriptor tables, who don't own the range pointers
			// We have a separate member that owns these ranges, referenced by the root parameters
			rsl::vector<rsl::vector<D3D12_DESCRIPTOR_RANGE>> ranges;
		};
	}
}