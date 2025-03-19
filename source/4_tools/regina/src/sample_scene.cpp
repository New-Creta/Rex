#include "regina/sample_scene.h"

#include "rex_engine/app/core_application.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/gfx/primitives/box.h"
#include "rex_engine/gfx/primitives/cylinder.h"
#include "rex_engine/gfx/primitives/grid.h"
#include "rex_engine/gfx/primitives/mesh_factory.h"
#include "rex_engine/gfx/primitives/sphere.h"
#include "rex_engine/gfx/primitives/quad.h"
#include "rex_engine/gfx/core/depth_info.h"
#include "rex_engine/gfx/core/vertex.h"
#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/system/gal.h"
#include "rex_std/bonus/math/color.h"
#include "rex_windows/app/gui_application.h"

#include <glm/gtc/matrix_transform.hpp>

#include "rex_engine/gfx/materials/material_system.h"
#include "rex_engine/gfx/components/mesh_component.h"
#include "rex_engine/components/transform_component.h"
#include "rex_engine/gfx/system/shader_library.h"

#include "rex_engine/images/internal/stb_image.h"

namespace regina
{
  SampleScene::SampleScene()
  {
    build_geometry();

    // input layout should be tied to the render item

    // shader and pso should be part of the material system
  }

  void SampleScene::build_geometry()
  {
    m_primitive_material = rex::gfx::load_material(rex::vfs::abs_path(rex::MountingPoint::EngineMaterials, "default.material"));

    s32 x, y, channels;
    stbi_uc* data = stbi_load(rex::path::join(rex::vfs::project_root(), "dev", "maps", "kanto", "old", "celadon_city.png").data(), &x, &y, &channels, 4);
    m_pokemon_texture = rex::gfx::gal()->create_texture2d(x, y, rex::gfx::TextureFormat::Unorm4, data);

    // Sampler is currently hardcoded
    rex::gfx::SamplerDesc desc{};
    desc.filtering = rex::gfx::SamplerFiltering::MinMagMipLinear;
    desc.address_mode_u = rex::gfx::TextureAddressMode::Wrap;
    desc.address_mode_v = rex::gfx::TextureAddressMode::Wrap;
    desc.address_mode_w = rex::gfx::TextureAddressMode::Wrap;
    desc.mip_lod_bias = 0.0f;
    desc.max_anisotropy = 0;
    desc.comparison_func = rex::gfx::ComparisonFunc::Always;
    desc.border_color = rex::gfx::BorderColor::TransparentBlack;
    desc.min_lod = 0.0f;
    desc.max_lod = 0.0f;
    desc.shader_register = 0;
    desc.register_space = 0;
    desc.shader_visibility = rex::gfx::ShaderVisibility::Pixel;

    m_pokemon_sampler = rex::gfx::gal()->create_sampler2d(desc);

    m_primitive_material->set("default_sampler", m_pokemon_sampler.get());
    m_primitive_material->set("default_texture", m_pokemon_texture.get());

    auto box      = rex::gfx::mesh_factory::create_box(1.5f, 1.5f, 1.5f);
    rex::gfx::Entity box_entity = add_entity();
    box_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(box)).set_material(m_primitive_material.get());
    box_entity.component<rex::gfx::TransformComponent>().scale *= 2.0f;

    auto quad     = rex::gfx::mesh_factory::create_quad(10.0f, 10.0f);
    rex::gfx::Entity quad_entity = add_entity();
    quad_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(quad)).set_material(m_primitive_material.get());
    quad_entity.component<rex::gfx::TransformComponent>().translation.z -= 2.0f;
    quad_entity.component<rex::gfx::TransformComponent>().translation.x -= 5.0f;
    quad_entity.component<rex::gfx::TransformComponent>().translation.y += 5.0f;
    quad_entity.component<rex::gfx::TransformComponent>().scale.x *= 0.8f;
    quad_entity.component<rex::gfx::TransformComponent>().scale.y *= 0.64f;
    

    // Spheres and Cylinders
    for(s32 i = 0; i < 5; ++i)
    {
      // Left
      create_pillar(glm::vec3(-5.0f, 0.0f, -10.0f + static_cast<f32>(i) * 5.0f));
      // Right
      create_pillar(glm::vec3(+5.0f, 0.0f, -10.0f + static_cast<f32>(i) * 5.0f));
    }
  }

  void SampleScene::create_pillar(const glm::vec3& pos)
  {
    rex::gfx::Entity sphere_entity = add_entity();
    rex::gfx::Entity cylinder_entity = add_entity();

    auto sphere = rex::gfx::mesh_factory::create_sphere(0.5f, 20, 20);
    auto cylinder = rex::gfx::mesh_factory::create_cylinder(0.5f, 0.3f, 3.0f, 20, 20);

    sphere_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(sphere)).set_material(m_primitive_material.get());
    cylinder_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(cylinder)).set_material(m_primitive_material.get());

    sphere_entity.component<rex::gfx::TransformComponent>().translation = pos;
    sphere_entity.component<rex::gfx::TransformComponent>().translation.y = 3.5f;

    cylinder_entity.component<rex::gfx::TransformComponent>().translation = pos;
    cylinder_entity.component<rex::gfx::TransformComponent>().translation.y = 1.5f;
  }


} // namespace regina
