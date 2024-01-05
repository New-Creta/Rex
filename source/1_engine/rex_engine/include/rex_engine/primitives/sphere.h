#pragma once

#include "rex_engine/primitives/mesh_factory.h"

#include <glm/gtc/constants.hpp>

namespace rex
{
    namespace mesh_factory
    {
				MeshData16 create_sphere(f32 radius, u16 sliceCount, u16 stackCount);

        MeshData16 create_geo_sphere(f32 radius, u32 numSubdivisions);
    }
}