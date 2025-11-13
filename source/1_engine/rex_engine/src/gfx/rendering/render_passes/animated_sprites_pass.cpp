#include "rex_engine/gfx/rendering/render_passes/animated_sprites_pass.h"

#include "rex_engine/filesystem/path.h"

#include "rex_engine/gfx/core/types.h"
#include "rex_engine/gfx/system/shader_library.h"
#include "rex_engine/gfx/rendering/tile_vertex.h"

#include "rex_engine/gfx/resources/animated_sprite.h"

#include "rex_engine/gfx/system/resource_manager.h"

#include "rex_engine/diagnostics/log.h"

// rendering animated data is pretty straightforward
// use the same principles as rendering the static tiles
// however we also have to store where we want to draw the tile
// as we can't assume it from the instance ID
// this location will be stored as a index and we can calculate
// the final position based on the inv tile size on the render target
// It's more likely that the texture will change in a given pass,
// so we also have to make sure we're using the correct texture for rendering
// this can be also done by storing an array of textures in the shader instead of a single one
// and the per instance data can also store which texture idx need to be used
// however, that last part is not implemented yet
// the texture is set per sprite and draw call at the moment

namespace rex
{
	namespace gfx
	{
		static const s32 FLIP_X_BIT = 0;
		static const s32 FLIP_Y_BIT = 1;
		static const s32 RENDER_BOTTOM_BEHIND_BG = 2;

		struct SceneRenderInfo
		{
			// screen info
			glm::vec2 inv_tile_screen_size;
			glm::vec2 screen_size;

			// sprite info
			glm::vec2 inv_sprite_screen_size;
			glm::vec2 inv_sprite_texture_size;

			glm::vec2 top_left_offset;
			glm::vec2 uv_start;

			s32 bit_masks;
		};

		AnimatedSpritesPass::AnimatedSpritesPass(const AnimatedSpritePassCreationInfo& creationInfo)
			: RenderPass(create_desc(creationInfo))
			, m_render_target(creationInfo.render_target)
		{
			init();
		}

		void AnimatedSpritesPass::update_scene_params(const SceneParams& params)
		{
			m_scene_params = params;
		}
		void AnimatedSpritesPass::update_camera_params(const CameraParams& params)
		{
			m_camera_params = params;

			auto render_ctx = gal::instance()->new_render_ctx();

			init_render_info(render_ctx.get());
		}

		AnimatedSprite* AnimatedSpritesPass::add_sprite(rsl::unique_ptr<AnimatedSprite> sprite)
		{
			m_sprites.emplace_back(rsl::move(sprite));
			return m_sprites.back().get();
		}

		void AnimatedSpritesPass::render(rex::gfx::RenderContext* renderCtx)
		{
			if (m_sprites.empty())
			{
				return;
			}
		
			bind_to(renderCtx);

			// Bind all the resources to the gfx pipeline
			renderCtx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_tiles_ib_gpu.get());

			static s32 counter = 0;

			s32 index_count = m_tiles_ib_gpu->count();
			for (const rsl::unique_ptr<AnimatedSprite>& sprite : m_sprites)
			{
				SceneRenderInfo scene_render_info{};
				static bool is_on_grass = false;
				if (counter % 60 == 0)
				{
					is_on_grass = !is_on_grass;
				}

				counter++;

				if (is_on_grass)
				{
					rsl::add_flag(scene_render_info.bit_masks, BIT(RENDER_BOTTOM_BEHIND_BG));
				}

				scene_render_info.inv_sprite_texture_size.x = 1.0f / (sprite->sprites_texture()->width() / sprite->sprite_size().x);
				scene_render_info.inv_sprite_texture_size.y = 1.0f / (sprite->sprites_texture()->height() / sprite->sprite_size().y);

				scene_render_info.top_left_offset = { 8.0f, 7.5f };

				rsl::pointi8 tile_size = m_scene_params.tileset->tile_size();
				rsl::pointi8 sprite_size = sprite->sprite_size();
				rsl::point<f32> inv_zoom_level{};
				inv_zoom_level.x = 1.0f / m_camera_params.zoom_level.x;
				inv_zoom_level.y = 1.0f / m_camera_params.zoom_level.y;

				rsl::pointi8 num_tiles_on_screen{};
				rsl::pointi8 num_sprites_on_screen{};

				num_tiles_on_screen.x = m_render_target->width() / (tile_size.x * m_camera_params.zoom_level.x);
				num_tiles_on_screen.y = m_render_target->height() / (tile_size.y * m_camera_params.zoom_level.y);

				num_sprites_on_screen.x = m_render_target->width() / (sprite_size.x * m_camera_params.zoom_level.x);
				num_sprites_on_screen.y = m_render_target->height() / (sprite_size.y * m_camera_params.zoom_level.y);

				f32 inv_tile_width = 2.0f / num_tiles_on_screen.x;
				f32 inv_tile_height = 2.0f / num_tiles_on_screen.y;

				f32 inv_sprite_width = 2.0f / num_sprites_on_screen.x;
				f32 inv_sprite_height = 2.0f / num_sprites_on_screen.y;

				scene_render_info.inv_sprite_screen_size.x = inv_sprite_width;
				scene_render_info.inv_sprite_screen_size.y = inv_sprite_height;

				scene_render_info.inv_tile_screen_size.x = inv_tile_width;
				scene_render_info.inv_tile_screen_size.y = inv_tile_height;

				scene_render_info.screen_size.x = m_render_target->width();
				scene_render_info.screen_size.y = m_render_target->height();

				rsl::point<f32> uv_start = sprite->current_sprite_uv();
				scene_render_info.uv_start.x = uv_start.x;
				scene_render_info.uv_start.y = uv_start.y;

				if (sprite->current_sprite().flip_x)
				{
					rsl::add_flag(scene_render_info.bit_masks, BIT(FLIP_X_BIT));
				}
				if (sprite->current_sprite().flip_y)
				{
					rsl::add_flag(scene_render_info.bit_masks, BIT(FLIP_Y_BIT));
				}

				renderCtx->update_buffer(m_tile_render_info.get(), &scene_render_info, sizeof(scene_render_info));

				set("sprite_texture", sprite->sprites_texture());
				bind_my_params_to_pipeline(renderCtx);

				renderCtx->draw_indexed(index_count, 0, 0, 0);
			}
		}

		void AnimatedSpritesPass::init()
		{
			REX_ASSERT_X(m_render_target != nullptr, "No render target provided for render pass");

			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_vb(render_ctx.get());
			init_ib(render_ctx.get());
			init_shader_params();
		}

		void AnimatedSpritesPass::init_vb(rex::gfx::RenderContext* renderCtx)
		{
			// Init the vertex buffer
			const s32 num_vertices_per_tile = 8;

			rsl::array<AnimatedTileVertex, num_vertices_per_tile> tile_vertices{};

			// sprites are drawn using 2 rectangles, splitting top and bottom
			// the top is always drawn on top of everything else
			// the bottom is sometimes drawn below the background
			// to give the illusion of 2.5D

			// top vertices
			tile_vertices[0] = AnimatedTileVertex{ glm::vec3(0, 0, 1),               rsl::point<f32>(0.0f, 0.0f) };
			tile_vertices[1] = AnimatedTileVertex{ glm::vec3(1,	0, 1),               rsl::point<f32>(1.0f, 0.0f) };

			// middle vertices
			tile_vertices[2] = AnimatedTileVertex{ glm::vec3(0,	-0.5f, 1),							 rsl::point<f32>(0.0f, 0.5f) };
			tile_vertices[3] = AnimatedTileVertex{ glm::vec3(1,	-0.5f, 1),							 rsl::point<f32>(1.0f, 0.5f) };

			tile_vertices[4] = AnimatedTileVertex{ glm::vec3(0,	-0.5f, 0),							 rsl::point<f32>(0.0f, 0.5f) };
			tile_vertices[5] = AnimatedTileVertex{ glm::vec3(1,	-0.5f, 0),							 rsl::point<f32>(1.0f, 0.5f) };

			// bottom vertices
			tile_vertices[6] = AnimatedTileVertex{ glm::vec3(0,	-1, 0),							 rsl::point<f32>(0.0f, 1.0f) };
			tile_vertices[7] = AnimatedTileVertex{ glm::vec3(1,	-1, 0),							 rsl::point<f32>(1.0f, 1.0f) };

			if (!m_tiles_vb_gpu)
			{
				m_tiles_vb_gpu = rex::gfx::gal::instance()->create_vertex_buffer(num_vertices_per_tile, sizeof(AnimatedTileVertex));
			}

			renderCtx->update_buffer(m_tiles_vb_gpu.get(), tile_vertices.data(), tile_vertices.size() * sizeof(AnimatedTileVertex));
			renderCtx->transition_buffer(m_tiles_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
		}
		void AnimatedSpritesPass::init_ib(rex::gfx::RenderContext* renderCtx)
		{
			if (m_tiles_ib_gpu)
			{
				return;
			}

			const s32 num_indices_per_tile = 12;
			rsl::array<u16, num_indices_per_tile > tile_ib{};

			// top rectangle
			tile_ib[0] = 0;	
			tile_ib[1] = 1;
			tile_ib[2] = 2;

			tile_ib[3] = 1;
			tile_ib[4] = 3;
			tile_ib[5] = 2;

			// bottom rectangle
			tile_ib[6] = 4;
			tile_ib[7] = 5;
			tile_ib[8] = 6;

			tile_ib[9] = 5;
			tile_ib[10] = 7;
			tile_ib[11] = 6;


			m_tiles_ib_gpu = rex::gfx::gal::instance()->create_index_buffer(num_indices_per_tile, rex::gfx::IndexBufferFormat::Uint16);

			// create the constant buffer
			// -----------------------------------------
			renderCtx->update_buffer(m_tiles_ib_gpu.get(), tile_ib.data(), tile_ib.size() * sizeof(tile_ib[0]));
			renderCtx->transition_buffer(m_tiles_ib_gpu.get(), rex::gfx::ResourceState::IndexBuffer);
		}
		void AnimatedSpritesPass::init_render_info(rex::gfx::RenderContext* renderCtx)
		{
			if (m_tile_render_info == nullptr)
			{
				m_tile_render_info = rex::gfx::gal::instance()->create_constant_buffer(sizeof(SceneRenderInfo));
				set("RenderingMetaData", m_tile_render_info.get());
			}
		}

		void AnimatedSpritesPass::init_shader_params()
		{
			rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);

			set("default_sampler", default_sampler);
			set("background_texture", (RenderTarget*)m_render_target);
		}

		RenderPassDesc AnimatedSpritesPass::create_desc(const AnimatedSpritePassCreationInfo& creationInfo) const
		{
			RenderPassDesc desc{};

			desc.name = "Animated Sprites Pass";

			desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);
			
			// Basic blending operation
			// If we output an alpha value of 0.0f
			// we use the pixel that's in the render target
			desc.pso_desc.output_merger.blend_state.render_target[0].blend_enable = true;
			desc.pso_desc.output_merger.blend_state.render_target[0].logic_op_enable = false;
			desc.pso_desc.output_merger.blend_state.render_target[0].src_blend = Blend::SrcAlpha;
			desc.pso_desc.output_merger.blend_state.render_target[0].dst_blend = Blend::InvSrcAlpha;
			desc.pso_desc.output_merger.blend_state.render_target[0].blend_op = BlendOp::Add;
			desc.pso_desc.output_merger.blend_state.render_target[0].src_blend_alpha = Blend::One;
			desc.pso_desc.output_merger.blend_state.render_target[0].dst_blend_alpha = Blend::Zero;
			desc.pso_desc.output_merger.blend_state.render_target[0].blend_op_alpha = BlendOp::Add;

			// We're rendering directly to the back buffer
			desc.framebuffer_desc.emplace_back(creationInfo.render_target);

			// Assign the shaders used for the tile renderer
			rex::scratch_string project_shaders = rex::path::join(rex::engine::instance()->project_root(), "shaders");
			desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(project_shaders, "animated_sprite.hlsl"), rex::gfx::ShaderType::Vertex);
			desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(project_shaders, "animated_sprite.hlsl"), rex::gfx::ShaderType::Pixel);

			desc.pso_desc.input_layout =
			{
				// Per vertex data
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::ShaderArithmeticType::Float3 },
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::ShaderArithmeticType::Float2 },
			};

			return desc;
		}
	}
}