#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/memory.h"

#include <glm/glm.hpp>

namespace rex
{
    class SceneNode;
    
    namespace mesh_factory
    {
        /**
         * Create a cube.
         *
         * @param size The size of one side of the cube.
         * @param reverseWinding Whether to reverse the winding order of the triangles (useful for skyboxes).
         */
        rsl::shared_ptr<SceneNode> create_cube(const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f), f32 size = 1.0);

        /**
         * Create a sphere.
         *
         * @param radius Radius of the sphere.
         * @param tessellation Determines how smooth the sphere is.
         * @param reverseWinding Whether to reverse the winding order of the triangles (useful for sydomes).
         */
        rsl::shared_ptr<SceneNode> create_sphere(const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f), f32 radius = 0.5f, u32 tessellation = 16);

        /**
         * Create a Cylinder
         *
         * @param radius The radius of the primary axis of the cylinder.
         * @param hight The height of the cylinder.
         * @param tessellation How smooth the cylinder will be.
         * @param reverseWinding Whether to reverse the winding order of the triangles.
         */
        rsl::shared_ptr<SceneNode> create_cylinder(const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f), f32 radius = 0.5f, f32 height = 1.0f, u32 tessellation = 32);

        /**
         * Create a plane.
         *
         * @param width The width of the plane.
         * @param height The height of the plane.
         * @reverseWinding Whether to reverse the winding order of the plane.
         */
        rsl::shared_ptr<SceneNode> create_plane(const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f), f32 width = 1.0f, f32 height = 1.0f);
    }
}