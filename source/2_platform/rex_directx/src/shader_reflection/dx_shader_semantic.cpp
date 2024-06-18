#include "rex_directx/shader_reflection/dx_shader_semantic.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/string/stringid.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace gfx
  {
    constexpr rsl::string_view g_Model = "Model";
    constexpr rsl::string_view g_View = "View";
    constexpr rsl::string_view g_Proj = "Proj";
    constexpr rsl::string_view g_MVP = "ModelViewProj";

    Semantic var_name_to_semantic(rsl::string_view name)
    {
      static rsl::unordered_map<rsl::string_view, Semantic> map =
      {
        { g_Model, Semantic::Model },
        { g_View, Semantic::View },
        { g_Proj, Semantic::Proj },
        { g_MVP, Semantic::MVP },
      };

      return map.at(name);
    }
  }
}