#include "rex_engine/gfx/rendering/render_passes/composite_pass.h"

#include "rex_engine/gfx/system/resource_manager.h"
#include "rex_engine/gfx/system/shader_library.h"

#include "rex_engine/filesystem/path.h"

namespace rex
{
	namespace gfx
	{
		struct CompositePassVertex
		{
			glm::vec2 pos;
			glm::vec2 uv;
		};

		CompositePass::CompositePass(const CompositePassCreationInfo& creationInfo)
			: RenderPass(create_desc(creationInfo))
			, m_src_render_target(creationInfo.src_render_target)
			, m_dst_render_target(creationInfo.dst_render_target)
		{
			init();
		}

		void CompositePass::render(rex::gfx::RenderContext* renderCtx)
		{
			bind_to(renderCtx);

			renderCtx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_tiles_ib_gpu.get());
			renderCtx->draw_indexed(m_tiles_ib_gpu->count(), 0, 0, 0);
		}

		RenderPassDesc CompositePass::create_desc(const CompositePassCreationInfo& creationInfo) const
		{
			RenderPassDesc desc{};

			desc.name = "Composite Pass";

			desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);
			//desc.pso_desc.output_merger.depth_stencil_state.depth_enable = true;
			//desc.pso_desc.output_merger.depth_stencil_state.depth_func = ComparisonFunc::Greater;
			//desc.pso_desc.output_merger.depth_stencil_state.depth_write_mask = DepthWriteMask::DepthWriteMaskAll;

			//desc.pso_desc.dsv_format = rex::gfx::resource_manager::instance()->find_depth_stencil_buffer("World Stencil Buffer")->format();

			// Basic blending operation
			// If we output an alpha value of 0.0f
			// we use the pixel that's in the render target
			//desc.pso_desc.output_merger.blend_state.render_target[0].blend_enable = true;
			//desc.pso_desc.output_merger.blend_state.render_target[0].logic_op_enable = false;
			//desc.pso_desc.output_merger.blend_state.render_target[0].src_blend = Blend::SrcAlpha;
			//desc.pso_desc.output_merger.blend_state.render_target[0].dst_blend = Blend::InvSrcAlpha;
			//desc.pso_desc.output_merger.blend_state.render_target[0].blend_op = BlendOp::Add;
			//desc.pso_desc.output_merger.blend_state.render_target[0].src_blend_alpha = Blend::One;
			//desc.pso_desc.output_merger.blend_state.render_target[0].dst_blend_alpha = Blend::Zero;
			//desc.pso_desc.output_merger.blend_state.render_target[0].blend_op_alpha = BlendOp::Add;

			// We're rendering directly to the back buffer
			desc.framebuffer_desc.emplace_back(creationInfo.dst_render_target);
			//desc.framebuffer_desc.emplace_back(rex::gfx::resource_manager::instance()->find_depth_stencil_buffer("World Stencil Buffer"));

			// Assign the shaders used for the tile renderer
			rex::scratch_string project_shaders = rex::path::join(rex::engine::instance()->project_root(), "shaders");
			desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(project_shaders, "composite_pass_vertex.hlsl"), rex::gfx::ShaderType::Vertex);
			desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(project_shaders, "composite_pass_pixel.hlsl"), rex::gfx::ShaderType::Pixel);

			desc.pso_desc.input_layout =
			{
				// Per vertex data
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::ShaderArithmeticType::Float2 },
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::ShaderArithmeticType::Float2 },
			};

			return desc;
		}

		void CompositePass::init()
		{
			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_vb(render_ctx.get());
			init_ib(render_ctx.get());
			init_shader_params();
		}

		void CompositePass::init_vb(RenderContext* renderCtx)
		{
			// Init the vertex buffer
			const s32 num_vertices_per_tile = 4;

			rsl::array<CompositePassVertex, num_vertices_per_tile> tile_vertices{};

			// sprites are drawn using 2 rectangles, splitting top and bottom
			// the top is always drawn on top of everything else
			// the bottom is sometimes drawn below the background
			// to give the illusion of 2.5D

			tile_vertices[0] = CompositePassVertex{ glm::vec2(-1, 1),                glm::vec2(0.0f, 0.0f) };
			tile_vertices[1] = CompositePassVertex{ glm::vec2(1,	1),               glm::vec2(1.0f, 0.0f) };
			tile_vertices[2] = CompositePassVertex{ glm::vec2(-1,	-1),							 glm::vec2(0.0f, 1.0f) };
			tile_vertices[3] = CompositePassVertex{ glm::vec2(1,	-1),							 glm::vec2(1.0f, 1.0f) };

			if (!m_tiles_vb_gpu)
			{
				m_tiles_vb_gpu = rex::gfx::gal::instance()->create_vertex_buffer(num_vertices_per_tile, sizeof(CompositePassVertex));
			}

			renderCtx->update_buffer(m_tiles_vb_gpu.get(), tile_vertices.data(), tile_vertices.size() * sizeof(CompositePassVertex));
			renderCtx->transition_buffer(m_tiles_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
		}
		void CompositePass::init_ib(RenderContext * renderCtx)
		{
			const s32 num_indices_per_tile = 6;
			rsl::array<u16, 6> tile_ib{};

			tile_ib[0] = 0;
			tile_ib[1] = 1;
			tile_ib[2] = 2;

			tile_ib[3] = 1;
			tile_ib[4] = 3;
			tile_ib[5] = 2;

			m_tiles_ib_gpu = rex::gfx::gal::instance()->create_index_buffer(num_indices_per_tile, rex::gfx::IndexBufferFormat::Uint16);

			// create the constant buffer
			// -----------------------------------------
			renderCtx->update_buffer(m_tiles_ib_gpu.get(), tile_ib.data(), tile_ib.size() * sizeof(tile_ib[0]));
			renderCtx->transition_buffer(m_tiles_ib_gpu.get(), rex::gfx::ResourceState::IndexBuffer);
		}
		void CompositePass::init_shader_params()
		{
			set("src_render_target", (RenderTarget*)m_src_render_target);

			rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);
			set("default_sampler", default_sampler);
		}

	}
}