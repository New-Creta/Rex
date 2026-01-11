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
			, m_src_shader_resource_view(nullptr)
			, m_src_render_target_texture(creationInfo.src_render_target)
			, m_dst_render_target_texture(creationInfo.dst_render_target)
		{
			init(creationInfo);
		}

		void CompositePass::render(rex::gfx::RenderContext* renderCtx)
		{
			bind_to(renderCtx);

			renderCtx->transition_buffer(m_src_render_target_texture, ResourceState::PixelShaderResource);

			renderCtx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_tiles_ib_gpu.get());
			renderCtx->draw_indexed(m_tiles_ib_gpu->count(), 0, 0, 0);
		}

		RenderPassDesc CompositePass::create_desc(const CompositePassCreationInfo& creationInfo) const
		{
			RenderPassDesc desc{};

			desc.name = "Composite Pass";

			desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);

			// We're rendering directly to the back buffer
			desc.framebuffer_desc.emplace_back(creationInfo.dst_render_target);

			// Assign the shaders used for the tile renderer
			rex::scratch_string project_shaders = rex::path::join(rex::engine::instance()->project_root(), "shaders");
			desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(project_shaders, "composite_pass.hlsl"), rex::gfx::ShaderType::Vertex);
			desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(project_shaders, "composite_pass.hlsl"), rex::gfx::ShaderType::Pixel);

			desc.pso_desc.input_layout =
			{
				// Per vertex data
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::ShaderArithmeticType::Float2 },
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::ShaderArithmeticType::Float2 },
			};

			return desc;
		}

		void CompositePass::init(const CompositePassCreationInfo& creationInfo)
		{
			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_vb(render_ctx.get());
			init_ib(render_ctx.get());
			init_shader_params(creationInfo);
		}

		void CompositePass::init_vb(RenderContext* renderCtx)
		{
			// Init the vertex buffer
			const s32 num_vertices_per_tile = 4;

			rsl::array<CompositePassVertex, num_vertices_per_tile> tile_vertices{};

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
		void CompositePass::init_shader_params(const CompositePassCreationInfo& creationInfo)
		{
			m_src_shader_resource_view = gal::instance()->create_srv(creationInfo.src_render_target);

			set("src_texture", m_src_shader_resource_view);

			rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);
			set("default_sampler", default_sampler);
		}

	}
}