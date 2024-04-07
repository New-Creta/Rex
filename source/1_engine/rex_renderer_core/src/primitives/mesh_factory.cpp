#include "rex_renderer_core/primitives/mesh_factory.h"

namespace rex
{
  namespace mesh_factory
  {
    //-----------------------------------------------------------------------
    VertexPosNormCol mid_point(const VertexPosNormCol& v0, const VertexPosNormCol& v1)
    {
      // Compute the midpoints of all the attributes.  Vectors need to be normalized
      // since linear interpolating can make them not unit length.
      VertexPosNormCol v;

      const glm::vec3 position = 0.5f * (v0.position() + v1.position());
      const glm::vec3 normal   = glm::normalize(0.5f * (v0.normal() + v1.normal()));
      const glm::vec4 color    = glm::normalize(0.5f * (v0.color() + v1.color()));

      return v;
    }
  } // namespace mesh_factory
} // namespace rex