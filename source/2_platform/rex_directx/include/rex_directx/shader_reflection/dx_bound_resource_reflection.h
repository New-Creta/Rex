#pragma once

#include "rex_std/bonus/string.h"
#include "rex_std/string_view.h"
#include <d3d12shader.h>

namespace rex
{
  namespace gfx
  {
    class DxBoundResourceReflection
    {
    public:
      DxBoundResourceReflection(const D3D12_SHADER_INPUT_BIND_DESC& resource_desc);

      rsl::string_view name() const;
      card32 shader_register() const;

    private:
      rsl::tiny_stack_string m_name;
      card32 m_register;
    };

  }
}