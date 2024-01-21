#include "rex_engine/primitives/mesh_factory.h"

namespace rex
{
    namespace mesh_factory
    {
        //-----------------------------------------------------------------------
        Vertex::Vertex()
            :position()
            , normal()
            , tangent()
            , uv()
        {}

        //-----------------------------------------------------------------------
        Vertex::Vertex(const glm::vec3& inPos, const glm::vec3& inNorm, const glm::vec3& inTang, const glm::vec2& inUV)
            :position(inPos)
            , normal(inNorm)
            , tangent(inTang)
            , uv(inUV)
        {}

        //-----------------------------------------------------------------------
        Vertex::Vertex(f32 xp, f32 yp, f32 zp, f32 xn, f32 yn, f32 zn, f32 xt, f32 yt, f32 zt, f32 xu, f32 yu)
            :position(xp, yp, zp)
            , normal(xn, yn, zn)
            , tangent(xt, yt, zt)
            , uv(xu, yu)
        {

        }

        //-----------------------------------------------------------------------
        Vertex mid_point(const Vertex& v0, const Vertex& v1)
        {
            // Compute the midpoints of all the attributes.  Vectors need to be normalized
            // since linear interpolating can make them not unit length.  
            Vertex v;

            v.position = 0.5f * (v0.position + v1.position);
            v.normal = glm::normalize(0.5f * (v0.normal + v1.normal));
            v.tangent = glm::normalize(0.5f * (v0.tangent + v1.tangent));
            v.uv = 0.5f * (v0.uv + v1.uv);

            return v;
        }
    }
}