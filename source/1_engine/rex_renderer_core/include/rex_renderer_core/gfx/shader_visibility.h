#pragma once

#include "rex_engine/engine/defines.h"
#include "rex_renderer_core/system/shader_type.h"
#include "rex_engine/engine/invalid_object.h"

namespace rex
{
  namespace gfx
  {
    REX_STATIC_WARNING("Combine shader type with shader visibility");

    // shader visibility is used to indicate which shader type has access to the specified resource
    enum class ShaderVisibility
    {
      Vertex = BIT(0),        // only the vertex shader has access to this resource
      Pixel = BIT(1),         // only the pixel shader has access to this resource
      Geometry = BIT(2),      // only the geometry shader has access to this resource
      Hull = BIT(3),          // only the hull shader has access to this resource
      Domain = BIT(4),        // only the domain shader has access to this resource
      Amplification = BIT(5), // only the amplification shader has access to this resource
      Mesh = BIT(6),          // only the mesh shader has access to this resource

      All = Vertex | Pixel | Geometry | Hull | Domain | Amplification | Mesh // all shaders have access to this resource
    };
    // Convert a shader type enum to the shader visibility flag it belongs to
    constexpr ShaderVisibility shader_type_to_visibility(ShaderType type)
    {
      switch (type)
      {
      case rex::gfx::ShaderType::Vertex:            return ShaderVisibility::Vertex;
      case rex::gfx::ShaderType::Pixel:             return ShaderVisibility::Pixel;
      case rex::gfx::ShaderType::Geometry:          return ShaderVisibility::Geometry;
      case rex::gfx::ShaderType::Hull:              return ShaderVisibility::Hull;
      case rex::gfx::ShaderType::Domain:            return ShaderVisibility::Domain;
      case rex::gfx::ShaderType::Amplification:     return ShaderVisibility::Amplification;
      case rex::gfx::ShaderType::Mesh:              return ShaderVisibility::Mesh;
      default: break;
      }

      return invalid_obj<ShaderVisibility>();
    }
  }
}