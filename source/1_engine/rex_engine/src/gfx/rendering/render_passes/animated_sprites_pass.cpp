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
		static const s32 MAX_SPRITES = 16;

		// Make sure this is 32 bits as on the GPU it's also 32 bits
		enum class SpriteRenderBits : s32
		{
			None										= 0,

			// If set, the sprite will be flipped, making left -> right and right -> left
			FlipX										= BIT(1),
			// If set, the sprite will be flipped, making up -> down and down -> up
			FlipY   								= BIT(2),
			// If set, the sprite's bottom half will be rendered behind the background
			RenderBottomBehindBg   	= BIT(3),
		};

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

			glm::vec2 uv_begin;

			SpriteRenderBits bit_masks;
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
			struct SpriteToRender
			{
				// The texture holding the sprite to render
				const Texture2D* sprites_texture;

				// The area within the texture containing the sprite to render
				glm::vec2 uv_begin;
				rsl::point<s8> size;

				// Information where to draw the sprite
				PixelCoord pos;

				// Extra metadata to indicate how to render the sprite
				SpriteRenderBits render_bits;
			};

			rsl::vector<SpriteToRender> sprites_to_render;
			for (const auto& sprite : m_sprites)
			{
				sprites_to_render.emplace_back(
					SpriteToRender{
						sprite->sprites_texture(),
					glm::vec2{sprite->current_sprite_uv().x, sprite->current_sprite_uv().y},
						sprite->sprite_size(),
						sprite->pos(),
						sprite->current_sprite().flip_x ? SpriteRenderBits::FlipX : SpriteRenderBits::None
					}
				);
			}

			if (sprites_to_render.empty())
			{
				return;
			}

			// Copy the render target to the background texture
			// so the background's pixels can be used if a sprite's bottom should be rendered behind it
			renderCtx->copy_rt_to_texture2d(m_render_target, m_background_texture.get());
			renderCtx->set_render_target(m_render_target);

			// Bind the pass to the render context
			bind_to(renderCtx);

			// Bind all the resources to the gfx pipeline
			renderCtx->set_vertex_buffer(m_sprite_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_sprite_ib_gpu.get());

			// Generate all the informated needed for the renderer to render this data and upload it to the gpu
			SceneRenderInfo scene_render_info{};
			scene_render_info.inv_vintage_pixel_screen_size.x = 2 * m_params.camera->zoom().x / m_render_target->width(); // how big is 1 tile pixel on the screen
			scene_render_info.inv_vintage_pixel_screen_size.y = 2 * m_params.camera->zoom().y / m_render_target->height(); // how big is 1 tile pixel on the screen

			scene_render_info.screen_size.x = static_cast<f32>(m_render_target->width());
			scene_render_info.screen_size.y = static_cast<f32>(m_render_target->height());
			renderCtx->update_buffer(m_screen_info_cbuffer.get(), &scene_render_info, sizeof(scene_render_info));
			for (s32 i = 0; i < m_sprites.size(); ++i)
			{
				const SpriteToRender& sprite = sprites_to_render[i];

				PerSpriteInstanceData per_instance_data{};

				// Needed to determine where to draw
				PixelCoord screen_pos = sprite.pos - m_params.camera->top_left();
				per_instance_data.screen_pos.x = screen_pos.x;
				per_instance_data.screen_pos.y = screen_pos.y;

				// See comment in wram.asm on line 91 related to Y screen position of wSpriteStateData1
				// ; - 4: Y screen position (in pixels, always 4 pixels above grid which makes sprites appear to be in the center of a tile)
				// The code that performs this offset if found in movement.asm
				// look for "add $4" as its done in a few places, depending on the executing code
				rsl::pointi8 tile_size = m_params.tileset->tile_size();
				per_instance_data.screen_pos.y -= tile_size.y / 2; // == 4

				rsl::pointi8 num_sprites_on_screen{};
				num_sprites_on_screen.x = narrow_cast<s8>(m_render_target->width() / (sprite.size.x * m_params.camera->zoom().x));
				num_sprites_on_screen.y = narrow_cast<s8>(m_render_target->height() / (sprite.size.y * m_params.camera->zoom().y));

				f32 inv_sprite_width = 2.0f / num_sprites_on_screen.x;
				f32 inv_sprite_height = 2.0f / num_sprites_on_screen.y;

				per_instance_data.inv_sprite_screen_size.x = inv_sprite_width;
				per_instance_data.inv_sprite_screen_size.y = inv_sprite_height;

				// Needed to determine what to draw
				per_instance_data.uv_begin								= sprite.uv_begin;
				per_instance_data.inv_sprite_texture_size.x = 1.0f / (sprite.sprites_texture->width() / sprite.size.x);
				per_instance_data.inv_sprite_texture_size.y = 1.0f / (sprite.sprites_texture->height() / sprite.size.y);

				// Needed to determine hwo to draw
				per_instance_data.bit_masks								= sprite.render_bits;

				renderCtx->update_buffer(m_per_instance_sb.get(), &per_instance_data, sizeof(per_instance_data), sizeof(per_instance_data) * i);

				set("sprite_texture", sprite.sprites_texture);
				bind_my_params_to_pipeline(renderCtx);
			}
			
			s32 index_count = m_sprite_ib_gpu->count();
			renderCtx->draw_indexed_instanced(index_count, m_sprites.size(), 0, 0, 0);
		}

		void AnimatedSpritesPass::init()
		{
			REX_ASSERT_X(m_render_target != nullptr, "No render target provided for render pass");

			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_vb(render_ctx.get());
			init_ib(render_ctx.get());
			init_render_info();
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
		void AnimatedSpritesPass::init_render_info()
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