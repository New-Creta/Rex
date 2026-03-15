#include "rex_engine/gfx/rendering/render_passes/animated_sprites_pass.h"

#include "rex_engine/filesystem/path.h"

#include "rex_engine/gfx/core/types.h"
#include "rex_engine/gfx/core/vertex.h"
#include "rex_engine/gfx/system/shader_library.h"

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
		static const s32 MAX_SPRITES = 16;

		using AnimatedTileVertex = VertexPosTex;

		struct SceneRenderInfo
		{
			// screen info
			glm::vec2 inv_vintage_pixel_screen_size;
			glm::vec2 screen_size;
		};

		struct PerSpriteInstanceData
		{
			rsl::pointi32 screen_pos; // unit represented in pixels

			// sprite info
			glm::vec2 inv_sprite_screen_size;
			glm::vec2 inv_sprite_texture_size;

			glm::vec2 uv_start;

			s32 bit_masks;
		};

		AnimatedSpritesPass::AnimatedSpritesPass(const AnimatedSpritePassCreationInfo& creationInfo)
			: RenderPass(create_desc(creationInfo))
			, m_render_target(creationInfo.render_target)
		{
			init();
		}

		AnimatedSprite* AnimatedSpritesPass::add_sprite(rsl::unique_ptr<AnimatedSprite> sprite)
		{
			m_sprites.emplace_back(rsl::move(sprite));
			return m_sprites.back().get();
		}

		void AnimatedSpritesPass::update_params(const SceneRenderParams& params)
		{
			m_params = params;
		}

		void AnimatedSpritesPass::render(rex::gfx::RenderContext* renderCtx)
		{
			if (m_sprites.empty())
			{
				return;
			}
		
			renderCtx->copy_rt_to_texture2d(m_background_texture.get(), m_render_target);
			renderCtx->set_render_target(m_render_target);

			bind_to(renderCtx);

			// Bind all the resources to the gfx pipeline
			renderCtx->set_vertex_buffer(m_sprite_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_sprite_ib_gpu.get());

			REX_STATIC_TODO("Detect if the player is on the grass tile dynamically");
			static s32 counter = 0;
			static bool is_on_grass = false;
			if (counter % 60 == 0)
			{
				is_on_grass = !is_on_grass;
			}

			counter++;
			
			SceneRenderInfo scene_render_info{};
			scene_render_info.inv_vintage_pixel_screen_size.x = 2 * m_params.camera->zoom().x / m_render_target->width(); // how big is 1 tile pixel on the screen
			scene_render_info.inv_vintage_pixel_screen_size.y = 2 * m_params.camera->zoom().y / m_render_target->height(); // how big is 1 tile pixel on the screen

			scene_render_info.screen_size.x = m_render_target->width();
			scene_render_info.screen_size.y = m_render_target->height();
			renderCtx->update_buffer(m_screen_info_cbuffer.get(), &scene_render_info, sizeof(scene_render_info));

			s32 index_count = m_sprite_ib_gpu->count();
			for (s32 i = 0; i < m_sprites.size(); ++i)
			{
				const rsl::unique_ptr<AnimatedSprite>& sprite = m_sprites[i];

				PerSpriteInstanceData per_instance_data{};

				if (is_on_grass)
				{
					rsl::add_flag(per_instance_data.bit_masks, BIT(RENDER_BOTTOM_BEHIND_BG));
				}

				per_instance_data.inv_sprite_texture_size.x = 1.0f / (sprite->sprites_texture()->width() / sprite->sprite_size().x);
				per_instance_data.inv_sprite_texture_size.y = 1.0f / (sprite->sprites_texture()->height() / sprite->sprite_size().y);

				rsl::pointi8 sprite_size = sprite->sprite_size();

				PixelCoord screen_pos = sprite->pos() - m_params.camera->top_left();
				per_instance_data.screen_pos.x = screen_pos.x;
				per_instance_data.screen_pos.y = screen_pos.y;

				// See comment in wram.asm on line 91 related to Y screen position of wSpriteStateData1
				// ; - 4: Y screen position (in pixels, always 4 pixels above grid which makes sprites appear to be in the center of a tile)
				// The code that performs this offset if found in movement.asm
				// look for "add $4" as its done in a few places, depending on the executing code
				rsl::pointi8 tile_size = m_params.tileset->tile_size();
				per_instance_data.screen_pos.y -= tile_size.y / 2; // == 4

				rsl::point<f32> inv_zoom_level{};
				inv_zoom_level.x = 1.0f / m_params.camera->zoom().x;
				inv_zoom_level.y = 1.0f / m_params.camera->zoom().y;

				rsl::pointi8 num_sprites_on_screen{};
				num_sprites_on_screen.x = m_render_target->width() / (sprite_size.x * m_params.camera->zoom().x);
				num_sprites_on_screen.y = m_render_target->height() / (sprite_size.y * m_params.camera->zoom().y);

				f32 inv_sprite_width = 2.0f / num_sprites_on_screen.x;
				f32 inv_sprite_height = 2.0f / num_sprites_on_screen.y;

				per_instance_data.inv_sprite_screen_size.x = inv_sprite_width;
				per_instance_data.inv_sprite_screen_size.y = inv_sprite_height;

				rsl::point<f32> uv_start = sprite->current_sprite_uv();
				per_instance_data.uv_start.x = uv_start.x;
				per_instance_data.uv_start.y = uv_start.y;

				if (sprite->current_sprite().flip_x)
				{
					rsl::add_flag(per_instance_data.bit_masks, BIT(FLIP_X_BIT));
				}
				if (sprite->current_sprite().flip_y)
				{
					rsl::add_flag(per_instance_data.bit_masks, BIT(FLIP_Y_BIT));
				}

				renderCtx->update_buffer(m_per_instance_sb.get(), &per_instance_data, sizeof(per_instance_data), sizeof(per_instance_data) * i);

				set("sprite_texture", sprite->sprites_texture());
				bind_my_params_to_pipeline(renderCtx);
			}
			
			renderCtx->draw_indexed_instanced(index_count, m_sprites.size(), 0, 0, 0);
		}

		void AnimatedSpritesPass::init()
		{
			REX_ASSERT_X(m_render_target != nullptr, "No render target provided for render pass");

			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_vb(render_ctx.get());
			init_ib(render_ctx.get());
			init_render_info(render_ctx.get());
			init_shader_params();
		}

		void AnimatedSpritesPass::init_vb(rex::gfx::RenderContext* renderCtx)
		{
			// Init the vertex buffer
			const s32 num_vertices_per_tile = 8;

			rsl::array<AnimatedTileVertex, num_vertices_per_tile> sprite_vertices{};

			// sprites are drawn using 2 rectangles, splitting top and bottom
			// the top is always drawn on top of everything else
			// the bottom is sometimes drawn below the background
			// to give the illusion of 2.5D

			// top vertices
			sprite_vertices[0] = AnimatedTileVertex{ glm::vec3(-1,   1, 1),    glm::vec2(0.0f, 0.0f) };
			sprite_vertices[1] = AnimatedTileVertex{ glm::vec3(1,	   1, 1),    glm::vec2(1.0f, 0.0f) };

			// middle vertices
			sprite_vertices[2] = AnimatedTileVertex{ glm::vec3(-1,	 0, 1),			glm::vec2(0.0f, 0.5f) };
			sprite_vertices[3] = AnimatedTileVertex{ glm::vec3(1,	   0, 1),			glm::vec2(1.0f, 0.5f) };

			sprite_vertices[4] = AnimatedTileVertex{ glm::vec3(-1,	 0, 0),			glm::vec2(0.0f, 0.5f) };
			sprite_vertices[5] = AnimatedTileVertex{ glm::vec3(1,	   0, 0),			glm::vec2(1.0f, 0.5f) };

			// bottom vertices
			sprite_vertices[6] = AnimatedTileVertex{ glm::vec3(-1,	-1, 0),			glm::vec2(0.0f, 1.0f) };
			sprite_vertices[7] = AnimatedTileVertex{ glm::vec3(1,	  -1, 0),			glm::vec2(1.0f, 1.0f) };

			if (!m_sprite_vb_gpu)
			{
				m_sprite_vb_gpu = rex::gfx::gal::instance()->create_vertex_buffer(num_vertices_per_tile, sizeof(AnimatedTileVertex));
			}

			renderCtx->update_buffer(m_sprite_vb_gpu.get(), sprite_vertices.data(), sprite_vertices.size() * sizeof(AnimatedTileVertex));
			renderCtx->transition_buffer(m_sprite_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
		}
		void AnimatedSpritesPass::init_ib(rex::gfx::RenderContext* renderCtx)
		{
			if (m_sprite_ib_gpu)
			{
				return;
			}

			const s32 num_indices_per_tile = 12;
			rsl::array<u16, num_indices_per_tile > sprite_ib{};

			// top rectangle
			sprite_ib[0] = 0;	
			sprite_ib[1] = 1;
			sprite_ib[2] = 2;

			sprite_ib[3] = 1;
			sprite_ib[4] = 3;
			sprite_ib[5] = 2;

			// bottom rectangle
			sprite_ib[6] = 4;
			sprite_ib[7] = 5;
			sprite_ib[8] = 6;

			sprite_ib[9] = 5;
			sprite_ib[10] = 7;
			sprite_ib[11] = 6;


			m_sprite_ib_gpu = rex::gfx::gal::instance()->create_index_buffer(num_indices_per_tile, rex::gfx::IndexBufferFormat::Uint16);

			// create the constant buffer
			// -----------------------------------------
			renderCtx->update_buffer(m_sprite_ib_gpu.get(), sprite_ib.data(), sprite_ib.size() * sizeof(sprite_ib[0]));
			renderCtx->transition_buffer(m_sprite_ib_gpu.get(), rex::gfx::ResourceState::IndexBuffer);
		}
		void AnimatedSpritesPass::init_render_info(rex::gfx::RenderContext* renderCtx)
		{
			if (m_screen_info_cbuffer == nullptr)
			{
				m_screen_info_cbuffer = rex::gfx::gal::instance()->create_constant_buffer(sizeof(SceneRenderInfo));
				set("RenderingMetaData", m_screen_info_cbuffer.get());
			}

			if (m_per_instance_sb == nullptr)
			{
				m_per_instance_sb = gal::instance()->create_structured_buffer(sizeof(PerSpriteInstanceData), s_max_allowed_instances);
				set("instance_data", m_per_instance_sb.get());
			}
		}
		void AnimatedSpritesPass::init_shader_params()
		{
			rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);

			set("default_sampler", default_sampler);

			m_background_texture = rex::gfx::gal::instance()->create_texture2d(m_render_target->width(), m_render_target->height(), TextureFormat::Unorm4Srgb);
			set("background_texture", m_background_texture.get());
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
			rex::scratch_string engine_shaders = rex::path::join(rex::engine::instance()->engine_root(), "shaders");
			desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(engine_shaders, "animated_sprite.hlsl"), rex::gfx::ShaderType::Vertex);
			desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(engine_shaders, "animated_sprite.hlsl"), rex::gfx::ShaderType::Pixel);

			desc.pso_desc.input_layout = AnimatedTileVertex::layout();

			return desc;
		}
	}
}