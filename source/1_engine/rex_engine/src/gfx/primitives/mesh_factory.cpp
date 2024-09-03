#include "rex_engine/gfx/primitives/mesh_factory.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      //-----------------------------------------------------------------------
      VertexPosNormTex mid_point(const VertexPosNormTex& v0, const VertexPosNormTex& v1)
      {
        // Compute the midpoints of all the attributes.  Vectors need to be normalized
        // since linear interpolating can make them not unit length.
        VertexPosNormTex v;

        const glm::vec3 position = 0.5f * (v0.position + v1.position);
        const glm::vec3 normal = glm::normalize(0.5f * (v0.normal + v1.normal));
        const glm::vec2 color = glm::normalize(0.5f * (v0.uv + v1.uv));

        return v;
      }

    } // namespace mesh_factory
  }
} // namespace rex