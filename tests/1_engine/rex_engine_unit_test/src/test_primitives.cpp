#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/primitives/box.h"
#include "rex_engine/primitives/cylinder.h"
#include "rex_engine/primitives/grid.h"
#include "rex_engine/primitives/sphere.h"

TEST_CASE("Box Creation Test")
{
    const f32 width = 2.0f;
    const f32 height = 2.0f;
    const f32 depth = 2.0f;
    const u32 subdivisions = 0;

    rex::mesh_factory::MeshData16 mesh_data = rex::mesh_factory::create_box(width, height, depth, subdivisions);

    SECTION("Check number of vertices and indices") 
    {
      REQUIRE(mesh_data.vertices().size() == 24);
      REQUIRE(mesh_data.indices().size() == 36);
    }
}

TEST_CASE("Cylinder Creation Test")
{
    const f32 bottomRadius = 1.0f;
    const f32 topRadius    = 1.0f;
    const f32 height       = 2.0f;
    const u16 sliceCount   = 10;
    const u16 stackCount   = 5;

    rex::mesh_factory::MeshData16 mesh_data = rex::mesh_factory::create_cylinder(bottomRadius, topRadius, height, sliceCount, stackCount);

    SECTION("Check number of vertices and indices")
    {
        REQUIRE(mesh_data.vertices().size() == ((sliceCount + 1) * (stackCount + 1) + 2 * (sliceCount + 1)) + 2);
        REQUIRE(mesh_data.indices().size() == (6 * sliceCount * (stackCount + 1)));
    }
}

TEST_CASE("Grid Creation Test")
{
    SECTION("Grid with valid parameters")
    {
        f32 width = 10.0f;
        f32 depth = 10.0f;
        u16 m       = 5;
        u16 n       = 5;

        rex::mesh_factory::MeshData16 mesh_data = rex::mesh_factory::create_grid(width, depth, m, n);

        REQUIRE(mesh_data.vertices().size() == m * n);
        REQUIRE(mesh_data.indices().size() == (m - 1) * (n - 1) * 6);
    }

    SECTION("Grid with zero width and depth")
    {
        f32 width = 0.0f;
        f32 depth = 0.0f;
        u16 m       = 5;
        u16 n       = 5;

        rex::mesh_factory::MeshData16 mesh_data = rex::mesh_factory::create_grid(width, depth, m, n);

        REQUIRE(mesh_data.vertices().size() == 0);
        REQUIRE(mesh_data.indices().size() == 0);
    }
}

TEST_CASE("Sphere Creation Test")
{
    const float radius   = 1.0f;
    const u16 sliceCount = 20;
    const u16 stackCount = 10;

    rex::mesh_factory::MeshData16 sphere = rex::mesh_factory::create_sphere(radius, sliceCount, stackCount);

    SECTION("Check number of vertices and indices")
    {
        REQUIRE(sphere.vertices().size() == (sliceCount + 1) * (stackCount - 1) + 2);
        REQUIRE(sphere.indices().size() == sliceCount * 6 * (stackCount - 2) + sliceCount * 3 * 2);
    }
}