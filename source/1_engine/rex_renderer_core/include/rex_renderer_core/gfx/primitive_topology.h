#pragma once

#include "rex_engine/engine/invalid_object.h"

namespace rex
{
  namespace gfx
  {
    enum class PrimitiveTopology
    {
      None = 0,
      PointList,    // A list of points
      LineList,     // A list of isolated straight line segments
      LineStrip,    // A primitive composed of connected line segments
      TriangleList, // A primitive of isolated triangles
      TriangleStrip // A primitive of connected triangles
    };

    enum class PrimitiveTopologyType
    {
      Undefined,
      Point,        
      Line,
      Triangle,
      Patch
    };

    // Convert primitive topology to its underlying type. This is used for the pso
    constexpr PrimitiveTopologyType to_topology_type(PrimitiveTopology topology)
    {
      switch (topology)
      {
      case rex::gfx::PrimitiveTopology::PointList:       return PrimitiveTopologyType::Point;
      case rex::gfx::PrimitiveTopology::LineList:        return PrimitiveTopologyType::Line;
      case rex::gfx::PrimitiveTopology::LineStrip:       return PrimitiveTopologyType::Line;
      case rex::gfx::PrimitiveTopology::TriangleList:    return PrimitiveTopologyType::Triangle;
      case rex::gfx::PrimitiveTopology::TriangleStrip:   return PrimitiveTopologyType::Triangle;
      default: break;
      }

      return invalid_obj<PrimitiveTopologyType>();
    }
  } // namespace gfx
} // namespace rex