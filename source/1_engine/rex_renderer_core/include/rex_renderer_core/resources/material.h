#pragma once

#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace gfx
  {
    class ShaderParam
    {};

    class RootSignature;

    class Material
    {
    public:
      Material(RootSignature* rootSignate);

      void set_param(rsl::string_view paramName, const void* data, rsl::memory_size size);

    private:
      RootSignature* m_root_signature;

      rsl::unordered_map<rsl::string_view, ShaderParam> m_shader_params;
    };
  }
}