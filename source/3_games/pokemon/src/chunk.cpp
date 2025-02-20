#include "pokemon/chunk.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/gfx/materials/material_system.h"
#include "rex_engine/gfx/system/gal.h"
#include "rex_engine/gfx/components/mesh_component.h"
#include "rex_engine/components/transform_component.h"
#include "rex_engine/gfx/primitives/mesh_factory.h"
#include "rex_engine/gfx/primitives/quad.h"

namespace pokemon
{
	Chunk::Chunk(rex::gfx::Entity entity, glm::vec3 pos)
		: m_entity(entity)
		, m_pos(pos)
	{
		//rsl::string background_path = rex::path::join(rex::vfs::project_root(), background);
		////asset_manager::load
		////m_material = rsl::make_unique<ChunkMaterial>(background_path);

		////auto quad = rex::gfx::mesh_factory::create_quad(width, height);
		////m_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(quad)).set_material(m_material.get());

		//m_material = rex::gfx::load_material(rex::vfs::abs_path(rex::MountingPoint::EngineMaterials, "default.material"));
		//s32 x, y, channels;
		//stbi_uc* data = stbi_load(background_path.data(), &x, &y, &channels, 0);
		//rex::gfx::TextureFormat format = rex::gfx::TextureFormat::Unorm3;
		//if (channels == 4)
		//{
		//	format = rex::gfx::TextureFormat::Unorm4;
		//}
		//m_texture = rex::gfx::gal::create_texture2d(x, y, format, data);
		//rex::gfx::SamplerDesc desc{};
		//desc.filtering = rex::gfx::SamplerFiltering::MinMagMipLinear;
		//desc.address_mode_u = rex::gfx::TextureAddressMode::Wrap;
		//desc.address_mode_v = rex::gfx::TextureAddressMode::Wrap;
		//desc.address_mode_w = rex::gfx::TextureAddressMode::Wrap;
		//desc.mip_lod_bias = 0.0f;
		//desc.max_anisotropy = 0;
		//desc.comparison_func = rex::gfx::ComparisonFunc::Always;
		//desc.border_color = rex::gfx::BorderColor::TransparentBlack;
		//desc.min_lod = 0.0f;
		//desc.max_lod = 0.0f;
		//desc.shader_register = 0;
		//desc.register_space = 0;
		//desc.shader_visibility = rex::gfx::ShaderVisibility::Pixel;
		//m_sampler = rex::gfx::gal::create_sampler2d(desc);

		//m_material->set("default_sampler", m_sampler.get());
		//m_material->set("default_texture", m_texture.get());

		//auto quad = rex::gfx::mesh_factory::create_quad(10.0f, 10.0f);
		//m_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(quad)).set_material(m_material.get());
		//m_entity.component<rex::gfx::TransformComponent>().translation += m_pos;
	}
}