#include "rex_directx/shader_reflection/dx_shader_param_type.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	namespace gfx
	{
		DXGI_FORMAT param_type_to_format(ParameterType type)
		{
      switch (type)
      {
      case ParameterType::Uint:				return DXGI_FORMAT_R32_UINT;
      case ParameterType::Uint2:				return DXGI_FORMAT_R32G32_UINT;
      case ParameterType::Uint16_2:		return DXGI_FORMAT_R16G16_UINT;
      case ParameterType::Float:				return DXGI_FORMAT_R32_FLOAT;
      case ParameterType::Float2:			return DXGI_FORMAT_R32G32_FLOAT;
      case ParameterType::Float3:			return DXGI_FORMAT_R32G32B32_FLOAT;
      case ParameterType::Float4:			return DXGI_FORMAT_R32G32B32A32_FLOAT;
      }

      REX_ASSERT("Invalid parameter type");
      return DXGI_FORMAT_UNKNOWN;
		}
	}
}