#include "pokemon/map.h"

#include "rex_renderer_core/primitives/box.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "rex_renderer_core/primitives/quad.h"
#include "rex_renderer_core/components/mesh_component.h"
#include "rex_renderer_core/components/transform_component.h"
#include "rex_renderer_core/materials/material_system.h"
#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/resources/sampler_2d.h"

#include "glm/glm.hpp"
#include "rex_engine/images/stb_image.h"

// Game Boy & Game Boy Color Resolution
// 160 x 144
// Tileset: 8x8
// 
// Player is always located at x=10,y=9
// with top left as origin
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   | x |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+


namespace pokemon
{

	Chunk::Chunk(rex::gfx::Entity& entity, rsl::string_view name, rsl::string_view background, glm::vec3 pos)
		: m_entity(entity)
		, m_pos(pos)
	{
		rsl::string background_path = rex::path::join(rex::vfs::project_root(), background);
		m_material = rex::gfx::load_material(rex::vfs::abs_path(rex::MountingPoint::EngineMaterials, "default.material"));
		s32 x, y, channels;
		stbi_uc* data = stbi_load(background_path.data(), &x, &y, &channels, 0);
		rex::gfx::TextureFormat format = rex::gfx::TextureFormat::Unorm3;
		if (channels == 4)
		{
			format = rex::gfx::TextureFormat::Unorm4;
		}
		m_texture = rex::gfx::rhi::create_texture2d(x, y, format, data);
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
		m_sampler = rex::gfx::rhi::create_sampler2d(desc);

		m_material->set("default_sampler", m_sampler.get());
		m_material->set("default_texture", m_texture.get());

		auto quad = rex::gfx::mesh_factory::create_quad(10.0f, 10.0f);
		m_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(quad)).set_material(m_material.get());
		m_entity.component<rex::gfx::TransformComponent>().translation += m_pos;
	}


	Map::Map(rex::gfx::Scene* scene, const rex::json::json& jsonBlob)
		: m_current_level_info(nullptr)
		, m_scene(scene)
	{
		m_name.assign(jsonBlob["name"].get<rsl::string_view>());

		const rex::json::json& levels_json = jsonBlob["levels"];
		for (const rex::json::json& level_json : levels_json)
		{
			LevelInfo level_info{};
			level_info.name.assign(level_json["name"].get<rsl::string_view>());
			level_info.background.assign(level_json["background"].get<rsl::string_view>());
			level_info.connections = level_json["connections"].get<rsl::vector<rsl::string>>();
			level_info.pos.x = level_json["position"]["x"].get<f32>();
			level_info.pos.y = level_json["position"]["y"].get<f32>();
			level_info.pos.z = level_json["position"]["z"].get<f32>();

			m_level_infos.emplace(level_info.name, level_info);
		}
	}

	void Map::start_level(rsl::string_view name)
	{
		const LevelInfo& level_info = m_level_infos.at(name);

		if (m_current_level_info)
		{
			m_chunks.clear();
		}

		// Do the following for every chunk
		m_chunks.emplace_back(m_scene->add_entity(), level_info.name, level_info.background, level_info.pos);

		m_current_level_info = &level_info;
	}
}