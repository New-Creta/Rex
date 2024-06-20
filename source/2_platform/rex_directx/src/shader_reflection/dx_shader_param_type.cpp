#include "rex_directx/shader_reflection/dx_shader_param_type.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	namespace gfx
	{
		DXGI_FORMAT param_type_to_format(ShaderParameterType type)
		{
      switch (type)
      {
      case ShaderParameterType::Uint:			  return DXGI_FORMAT_R32_UINT;
      case ShaderParameterType::Uint2:		  return DXGI_FORMAT_R32G32_UINT;
      case ShaderParameterType::Uint16_2:	  return DXGI_FORMAT_R16G16_UINT;
      case ShaderParameterType::Float:		  return DXGI_FORMAT_R32_FLOAT;
      case ShaderParameterType::Float2:		  return DXGI_FORMAT_R32G32_FLOAT;
      case ShaderParameterType::Float3:		  return DXGI_FORMAT_R32G32B32_FLOAT;
      case ShaderParameterType::Float4:		  return DXGI_FORMAT_R32G32B32A32_FLOAT;
      }

      REX_ASSERT("Invalid parameter type");
      return DXGI_FORMAT_UNKNOWN;
		}
	}
}