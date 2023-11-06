#include "rex_directx/directx_util.h"
#include "rex_directx/utility/vertex.h"
#include "rex_directx/wrl/wrl_types.h"

#include "rex_renderer_core/renderer.h"

#include "rex_engine/core_application.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/windowinfo.h"

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
        DirectX::XMFLOAT4X4 world_view_proj = math_helper::Identity4x4();
    };

    struct ReginaContext
    {
        rsl::unique_ptr<Mesh> mesh_cube = nullptr;

        s32 shader_program;
        s32 input_layout;
        s32 constant_buffer;
        s32 pso;

        DirectX::XMFLOAT4X4 world = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 view = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 proj = math_helper::Identity4x4();
    };

    ReginaContext g_regina_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

    bool build_shader_and_input_layout()
    {
        // Shader
        renderer::parameters::CompileShader vs_compile_params;
        renderer::parameters::CompileShader ps_compile_params;
        s32 vertex_shader = renderer::compile_shader(vs_compile_params);
        s32 pixel_shader = renderer::compile_shader(ps_compile_params);
        renderer::parameters::LinkShader link_shader_params;
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

        renderer::parameters::CreateBuffer v_create_buffer_params;
        g_regina_ctx.mesh_cube->vertex_buffer = renderer::create_vertex_buffer(v_create_buffer_params);
        renderer::parameters::CreateBuffer i_create_buffer_params;
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

    bool build_constant_buffers()
    {
        renderer::parameters::CreateConstantBuffer create_const_buffer_params;
        g_regina_ctx.constant_buffer = renderer::create_constant_buffer(create_const_buffer_params);

        return true;
    }

    bool build_pipeline_state_object()
    {
        renderer::parameters::CreatePipelineState create_pso_params;
        g_regina_ctx.pso = renderer::create_pipeline_state_object(create_pso_params);

        return true;
    }

    bool initialize()
    {
        REX_LOG(LogRegina, "Initializing Regina");

        if (!build_shader_and_input_layout()) return false;
        if (!build_cube_geometry()) return false;
        if (!build_constant_buffers()) return false;
        if (!build_pipeline_state_object()) return false;

        // The window resized, so update the aspect ratio and recompute the projection matrix.
        f32 aspect_ratio = static_cast<f32>(globals::window_info().width) / static_cast<f32>(globals::window_info().height);
        DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, aspect_ratio, 1.0f, 1000.0f);
        DirectX::XMStoreFloat4x4(&g_regina_ctx.proj, P);

        return true;
    }
    void update()
    {
        // Convert Spherical to Cartesian coordinates.
        float x = 5.0f * sinf(DirectX::XM_PIDIV4) * cosf(1.5f * DirectX::XM_PI);
        float z = 5.0f * sinf(DirectX::XM_PIDIV4) * sinf(1.5f * DirectX::XM_PI);
        float y = 5.0f * cosf(DirectX::XM_PIDIV4);

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
    void draw()
    {
        renderer::backend::new_frame();

        renderer::backend::clear();

        ID3D12DescriptorHeap* descriptor_heaps[] = { g_regina_ctx.cbv_heap.Get() };
        get_dx12_command_list()->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);

        get_dx12_command_list()->SetGraphicsRootSignature(g_regina_ctx.root_signature.Get());

        auto vertex_buffer_view = g_regina_ctx.mesh_cube->vertex_buffer_view();
        auto index_buffer_view = g_regina_ctx.mesh_cube->index_buffer_view();
        get_dx12_command_list()->IASetVertexBuffers(0, 1, &vertex_buffer_view);
        get_dx12_command_list()->IASetIndexBuffer(&index_buffer_view);
        get_dx12_command_list()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        get_dx12_command_list()->SetGraphicsRootDescriptorTable(0, g_regina_ctx.cbv_heap->GetGPUDescriptorHandleForHeapStart());

        get_dx12_command_list()->DrawIndexedInstanced(g_regina_ctx.mesh_cube->draw_args["box"].index_count, 1, 0, 0, 0);

        renderer::backend::present();
        renderer::backend::end_frame();
    }
    void shutdown()
    {
        REX_LOG(LogRegina, "shutting down Regina");

        g_regina_ctx.root_signature.Reset();
        g_regina_ctx.cbv_heap.Reset();

        g_regina_ctx.object_constant_buffer.reset();

        g_regina_ctx.mesh_cube.reset();

        g_regina_ctx.vertex_shader_byte_code.Reset();
        g_regina_ctx.pixel_shader_byte_code.Reset();
    }

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

    ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
    {
        return create_regina_app_creation_params(rsl::move(platformParams));
    }
#endif
} // namespace rex
