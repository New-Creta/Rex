#include "rex_directx/directx_util.h"
#include "rex_directx/utility/vertex.h"
#include "rex_directx/wrl/wrl_types.h"

#include "rex_renderer_core/renderer.h"

#include "rex_engine/core_application.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/windowinfo.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_std/string.h"
#include "rex_std_extra/memory/memory_size.h"

#include "rex_windows/gui_application.h"
#include "rex_windows/platform_creation_params.h"

#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>

DEFINE_LOG_CATEGORY(LogRegina, rex::LogVerbosity::Log);

namespace rex
{
    namespace math_helper
    {
        DirectX::XMFLOAT4X4 Identity4x4()
        {
            static DirectX::XMFLOAT4X4 I(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

            return I;
        }
    } // namespace math_helper

    // Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
    // geometries are stored in one vertex and index buffer.  It provides the offsets
    // and data needed to draw a subset of geometry stores in the vertex and index
    // buffers so that we can implement the technique described by Figure 6.3.
    struct Submesh
    {
        s32 index_count = 0;
        s32 start_index_location = 0;
        s32 base_vertex_location = 0;

        // Bounding box of the geometry defined by this submesh.
        // This is used in later chapters of the book.
        DirectX::BoundingBox bounds;
    };

    struct Mesh
    {
        ~Mesh()
        {
            renderer::release_resource(vertex_buffer);
            renderer::release_resource(index_buffer);
        }

        // Give it a name so we can look it up by name.
        rsl::string name;

        // System memory copies.  Use Blobs because the vertex/index format can be generic.
        // It is up to the client to cast appropriately.
        s32 vertex_buffer;
        s32 index_buffer;

        // Data about the buffers.
        s32 vertex_byte_stride = 0;
        s32 vertex_buffer_byte_size = 0;
        renderer::IndexBufferFormat index_format = renderer::IndexBufferFormat::R16_UINT;
        s32 index_buffer_byte_size = 0;

        // A MeshGeometry may store multiple geometries in one vertex/index buffer.
        // Use this container to define the Submesh geometries so we can draw
        // the Submeshes individually.
        std::unordered_map<std::string, Submesh> draw_args;
    };

    struct ObjectConstants
    {
        DirectX::XMFLOAT4X4 world = math_helper::Identity4x4();
    };

    struct PassConstants
    {
        DirectX::XMFLOAT4X4 view = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 inv_view = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 proj = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 inv_proj = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 view_proj = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 inv_view_proj = math_helper::Identity4x4();

        DirectX::XMFLOAT3 eye_pos_w = { 0.0f, 0.0f, 0.0f };
        f32 cb_padding_1 = 0.0f;

        DirectX::XMFLOAT2 render_target_size = { 0.0f, 0.0f };
        DirectX::XMFLOAT2 inv_render_target_size = { 0.0f, 0.0f };

        f32 near_z = 0.0f;
        f32 far_z = 0.0f;

        f32 total_time = 0.0f;
        f32 delta_time = 0.0f;
    };

    struct ReginaContext
    {
        rsl::unique_ptr<Mesh> mesh_cube = nullptr;

        s32 clear_state;

        s32 shader_program;
        s32 input_layout;
        s32 pso;

        std::vector<s32> object_constant_buffers;
        std::vector<s32> pass_constants_buffers;

        s32 solid_raster_state;
        s32 wire_raster_state;

        DirectX::XMFLOAT4X4 world = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 view = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 proj = math_helper::Identity4x4();
    };

    ReginaContext g_regina_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

    //-------------------------------------------------------------------------
    renderer::parameters::CompileShader create_compile_shader_parameters(const rsl::small_stack_string& shaderName, renderer::ShaderType shaderType, rsl::string_view filePath)
    {
        memory::Blob file_memory = vfs::open_read(filePath);

        renderer::parameters::CompileShader compile_shader_params;

        char8* entry_point;
        char8* target;

        switch (shaderType)
        {
        case renderer::ShaderType::VERTEX:
            entry_point = "VS";
            target = "vs_5_1";
            break;
        case renderer::ShaderType::PIXEL:
            entry_point = "PS";
            target = "ps_5_1";
            break;
        }

        compile_shader_params.shader_code = file_memory.data_as<char8>();
        compile_shader_params.shader_code_size = file_memory.size();
        compile_shader_params.shader_entry_point = entry_point;
        compile_shader_params.shader_feature_target = target;
        compile_shader_params.shader_name = shaderName;
        compile_shader_params.shader_type = shaderType;

        return compile_shader_params;
    }

    //-------------------------------------------------------------------------
    template<typename T>
    renderer::parameters::CreateBuffer create_buffer_parameters(T* data, s32 num)
    {
        renderer::parameters::CreateBuffer create_buffer_params;

        create_buffer_params.data = (void*)&data[0];
        create_buffer_params.buffer_size = sizeof(T) * num;

        return create_buffer_params;
    }

    //-------------------------------------------------------------------------
    bool build_clear_state()
    {
        renderer::parameters::ClearState create_clear_state_params;

        create_clear_state_params.rgba = { 0.0f, 0.0, 0.5f, 1.0f };
        create_clear_state_params.depth = 1.0f;
        create_clear_state_params.stencil = 0x00;

        StateController<renderer::ClearBits> clear_flags;
        clear_flags.add_state(renderer::ClearBits::CLEAR_COLOR_BUFFER);
        clear_flags.add_state(renderer::ClearBits::CLEAR_DEPTH_BUFFER);
        clear_flags.add_state(renderer::ClearBits::CLEAR_STENCIL_BUFFER);

        create_clear_state_params.flags = clear_flags;;

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_shader_and_input_layout()
    {
        // Shader
        renderer::parameters::ConstantLayoutDescription constants[2] = 
        {
            {renderer::parameters::ConstantType::CBUFFER, "ObjectConstants", 0},
            {renderer::parameters::ConstantType::CBUFFER, "PassConstants", 1}
        };

        renderer::parameters::CompileShader vs_compile_params = create_compile_shader_parameters(rsl::small_stack_string("standardVS"), renderer::ShaderType::VERTEX, "Shaders\\color.hlsl");
        renderer::parameters::CompileShader ps_compile_params = create_compile_shader_parameters(rsl::small_stack_string("opaquePS"), renderer::ShaderType::VERTEX, "Shaders\\color.hlsl");
        renderer::parameters::LinkShader link_shader_params;
        link_shader_params.vertex_shader = renderer::compile_shader(vs_compile_params);
        link_shader_params.pixel_shader = renderer::compile_shader(ps_compile_params);
        link_shader_params.constants = constants;
        link_shader_params.num_constants = _countof(constants);
        g_regina_ctx.shader_program = renderer::link_shader(link_shader_params);

        // Input Layout
        renderer::parameters::InputLayoutDescription input_layout_descs[2] =
        {
            { "POSITION", 0, renderer::VertexBufferFormat::FLOAT3, 0, 0, renderer::InputLayoutClassification::PER_VERTEX_DATA, 0 },
            { "COLOR", 0, renderer::VertexBufferFormat::FLOAT4, 0, 12, renderer::InputLayoutClassification::PER_VERTEX_DATA, 0 }
        };

        renderer::parameters::CreateInputLayout input_layout_params;
        input_layout_params.input_layout = input_layout_descs;
        input_layout_params.num_elements = _countof(input_layout_descs);
        g_regina_ctx.input_layout = renderer::create_input_layout(input_layout_params);
    }

    //-------------------------------------------------------------------------
    bool build_cube_geometry()
    {
        rsl::array<renderer::directx::VertexPosCol, 8> vertices =
        {
            renderer::directx::VertexPosCol({ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::White) }),
            renderer::directx::VertexPosCol({ DirectX::XMFLOAT3(-1.0f, +1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Black) }),
            renderer::directx::VertexPosCol({ DirectX::XMFLOAT3(+1.0f, +1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Red) }),
            renderer::directx::VertexPosCol({ DirectX::XMFLOAT3(+1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Green) }),
            renderer::directx::VertexPosCol({ DirectX::XMFLOAT3(-1.0f, -1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Blue) }),
            renderer::directx::VertexPosCol({ DirectX::XMFLOAT3(-1.0f, +1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Yellow) }),
            renderer::directx::VertexPosCol({ DirectX::XMFLOAT3(+1.0f, +1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Cyan) }),
            renderer::directx::VertexPosCol({ DirectX::XMFLOAT3(+1.0f, -1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Magenta) })
        };

        rsl::array<u16, 36> indices =
        {
            // front face
            0, 1, 2,
            0, 2, 3,

            // back face
            4, 6, 5,
            4, 7, 6,

            // left face
            4, 5, 1,
            4, 1, 0,

            // right face
            3, 2, 6,
            3, 6, 7,

            // top face
            1, 5, 6,
            1, 6, 2,

            // bottom face
            4, 0, 3,
            4, 3, 7
        };

        const u32 vb_byte_size = (u32)vertices.size() * sizeof(renderer::directx::VertexPosCol);
        const u32 ib_byte_size = (u32)indices.size() * sizeof(u16);

        g_regina_ctx.mesh_cube = rsl::make_unique<Mesh>();
        g_regina_ctx.mesh_cube->name = rsl::string("box_geometry");

        renderer::parameters::CreateBuffer v_create_buffer_params = create_buffer_parameters<renderer::directx::VertexPosCol>(vertices.data(), vertices.size());
        g_regina_ctx.mesh_cube->vertex_buffer = renderer::create_vertex_buffer(v_create_buffer_params);
        renderer::parameters::CreateBuffer i_create_buffer_params = create_buffer_parameters<u16>(indices.data(), indices.size());
        g_regina_ctx.mesh_cube->index_buffer = renderer::create_index_buffer(i_create_buffer_params);

        g_regina_ctx.mesh_cube->vertex_byte_stride = sizeof(renderer::directx::VertexPosCol);
        g_regina_ctx.mesh_cube->vertex_buffer_byte_size = vb_byte_size;
        g_regina_ctx.mesh_cube->index_format = renderer::IndexBufferFormat::R16_UINT;
        g_regina_ctx.mesh_cube->index_buffer_byte_size = ib_byte_size;

        Submesh submesh;
        submesh.index_count = (UINT)indices.size();
        submesh.start_index_location = 0;
        submesh.base_vertex_location = 0;

        g_regina_ctx.mesh_cube->draw_args["box"] = submesh;

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_constant_buffers()
    {
        s32 num_render_items = 1;
        s32 num_frame_resources = 1;

        // Need a CBV descriptor for each object for each frame resource.
        for (s32 frame = 0; frame < num_frame_resources; ++frame)
        {
            renderer::parameters::CreateConstantBuffer create_const_buffer_params;

            create_const_buffer_params.count = num_render_items;
            create_const_buffer_params.data = nullptr;
            create_const_buffer_params.buffer_size = sizeof(ObjectConstants);

            s32 constant_buffer = renderer::create_constant_buffer(create_const_buffer_params);

            g_regina_ctx.object_constant_buffers.push_back(constant_buffer);
        }

        // Last three descriptors are the pass CBVs for each frame resource.
        for (s32 frame = 0; frame < num_frame_resources; ++frame)
        {
            renderer::parameters::CreateConstantBuffer create_const_buffer_params;

            create_const_buffer_params.count = 1;
            create_const_buffer_params.data = nullptr;
            create_const_buffer_params.buffer_size = sizeof(PassConstants);

            s32 constant_buffer = renderer::create_constant_buffer(create_const_buffer_params);

            g_regina_ctx.pass_constants_buffers.push_back(constant_buffer);
        }

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_raster_state()
    {
        renderer::parameters::RasterState solid_rs;
        solid_rs.fill_mode = renderer::FillMode::SOLID;
        solid_rs.cull_mode = renderer::CullMode::BACK;
        g_regina_ctx.solid_raster_state = renderer::create_raster_state(solid_rs);

        renderer::parameters::RasterState wire_rs;
        wire_rs.fill_mode = renderer::FillMode::SOLID;
        wire_rs.cull_mode = renderer::CullMode::BACK;
        g_regina_ctx.wire_raster_state = renderer::create_raster_state(wire_rs);

        return true;
    }

    //-------------------------------------------------------------------------
    bool build_pipeline_state_object()
    {
        renderer::parameters::CreatePipelineState create_pso_params;
        create_pso_params.input_layout = g_regina_ctx.input_layout;
        create_pso_params.num_render_targets = 1;
        create_pso_params.shader_program = g_regina_ctx.shader_program;
        create_pso_params.rasterizer_state = g_regina_ctx.solid_raster_state;
        g_regina_ctx.pso = renderer::create_pipeline_state_object(create_pso_params);

        return true;
    }

    //-------------------------------------------------------------------------
    bool initialize()
    {
        REX_LOG(LogRegina, "Initializing Regina");

        if (!build_clear_state()) return false;
        if (!build_shader_and_input_layout()) return false;
        if (!build_cube_geometry()) return false;
        if (!build_constant_buffers()) return false;
        if (!build_raster_state()) return false;
        if (!build_pipeline_state_object()) return false;

        // The window resized, so update the aspect ratio and recompute the projection matrix.
        f32 aspect_ratio = static_cast<f32>(globals::window_info().width) / static_cast<f32>(globals::window_info().height);
        DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, aspect_ratio, 1.0f, 1000.0f);
        DirectX::XMStoreFloat4x4(&g_regina_ctx.proj, P);

        return true;
    }
    //-------------------------------------------------------------------------
    void update()
    {
        // Convert Spherical to Cartesian coordinates.
        f32 x = 5.0f * sinf(DirectX::XM_PIDIV4) * cosf(1.5f * DirectX::XM_PI);
        f32 z = 5.0f * sinf(DirectX::XM_PIDIV4) * sinf(1.5f * DirectX::XM_PI);
        f32 y = 5.0f * cosf(DirectX::XM_PIDIV4);

        // Build the view matrix.
        DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
        DirectX::XMVECTOR target = DirectX::XMVectorZero();
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
        DirectX::XMStoreFloat4x4(&g_regina_ctx.view, view);

        DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&g_regina_ctx.world);
        DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&g_regina_ctx.proj);
        DirectX::XMMATRIX wvp = world * view * proj;

        // Update the constant buffer with the latest worldViewProj matrix.
        ObjectConstants obj_constants;
        DirectX::XMStoreFloat4x4(&obj_constants.world_view_proj, DirectX::XMMatrixTranspose(wvp));
        g_regina_ctx.object_constant_buffer->copy_data(0, obj_constants);
    }
    //-------------------------------------------------------------------------
    void draw()
    {
        Viewport vp = { 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f };
        ScissorRect sr = { vp.top_left_x, vp.top_left_y, vp.width, vp.height };

        renderer::set_pipeline_state_object(g_regina_ctx.pso);
        renderer::new_frame();

        renderer::set_render_targets((s32)renderer::DefaultTargets::REX_BACK_BUFFER_COLOR, (s32)renderer::DefaultTargets::REX_BUFFER_DEPTH);
        renderer::set_viewport(vp);
        renderer::set_scissor_rect(sr);
        renderer::set_raster_state(g_regina_ctx.solid_raster_state);

        renderer::begin_draw();

        renderer::clear(g_regina_ctx.clear_state);

        renderer::set_shader(g_regina_ctx.shader_program);
        renderer::set_constant_buffer(0, 0);
        renderer::set_vertex_buffer(g_regina_ctx.mesh_cube->vertex_buffer, 0, 0, 0);
        renderer::set_index_buffer(g_regina_ctx.mesh_cube->index_buffer, renderer::IndexBufferFormat::R16_UINT, 0);

        renderer::renderer_draw_indexed_instanced(g_regina_ctx.mesh_cube->draw_args["box"].index_count, 1, 0, 0, 0, renderer::PrimitiveTopology::TRIANGLELIST);

        renderer::end_draw();

        renderer::backend::present();
        renderer::backend::end_frame();
    }
    //-------------------------------------------------------------------------
    void shutdown()
    {
        REX_LOG(LogRegina, "shutting down Regina");

        renderer::release_resource(g_regina_ctx.clear_state);

        renderer::release_resource(g_regina_ctx.shader_program);
        renderer::release_resource(g_regina_ctx.input_layout);
        renderer::release_resource(g_regina_ctx.pso);

        for(s32 ocb: g_regina_ctx.object_constant_buffers)
        {
            renderer::release_resource(ocb);
        }
        for(s32 pcb: g_regina_ctx.pass_constants_buffers)
        {
            renderer::release_resource(pcb);
        }

        renderer::release_resource(g_regina_ctx.solid_raster_state);
        renderer::release_resource(g_regina_ctx.wire_raster_state);

        g_regina_ctx.mesh_cube.reset();
    }

    //-------------------------------------------------------------------------
    ApplicationCreationParams create_regina_app_creation_params(PlatformCreationParams&& platformParams)
    {
        ApplicationCreationParams app_params(rsl::move(platformParams));

        app_params.gui_params.window_width = 1280;
        app_params.gui_params.window_height = 720;
        app_params.gui_params.window_title = "Regina";

        app_params.engine_params.max_memory = 256_kb;
        app_params.engine_params.app_init_func = initialize;
        app_params.engine_params.app_update_func = update;
        app_params.engine_params.app_draw_func = draw;
        app_params.engine_params.app_shutdown_func = shutdown;

        app_params.create_window = true;

        return app_params;
    }

#ifndef REX_ENABLE_AUTO_TESTS
    //-------------------------------------------------------------------------
    ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
    {
        return create_regina_app_creation_params(rsl::move(platformParams));
    }

#endif
} // namespace rex
