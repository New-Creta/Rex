#include "rex_directx/directx_util.h"
#include "rex_directx/resources/upload_buffer.h"
#include "rex_directx/resources/vertex.h"
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
        u32 index_count = 0;
        u32 start_index_location = 0;
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
        wrl::com_ptr<ID3DBlob> vertex_buffer_cpu = nullptr;
        wrl::com_ptr<ID3DBlob> index_buffer_cpu = nullptr;

        wrl::com_ptr<ID3D12Resource> vertex_buffer_gpu = nullptr;
        wrl::com_ptr<ID3D12Resource> index_buffer_gpu = nullptr;

        wrl::com_ptr<ID3D12Resource> vertex_buffer_uploader = nullptr;
        wrl::com_ptr<ID3D12Resource> index_buffer_uploader = nullptr;

        // Data about the buffers.
        u32 vertex_byte_stride = 0;
        u32 vertex_buffer_byte_size = 0;
        DXGI_FORMAT index_format = DXGI_FORMAT_R16_UINT;
        u32 index_buffer_byte_size = 0;

        // A MeshGeometry may store multiple geometries in one vertex/index buffer.
        // Use this container to define the Submesh geometries so we can draw
        // the Submeshes individually.
        std::unordered_map<std::string, Submesh> draw_args;

        D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view() const
        {
            D3D12_VERTEX_BUFFER_VIEW vbv;

            vbv.BufferLocation = vertex_buffer_gpu->GetGPUVirtualAddress();
            vbv.StrideInBytes = vertex_byte_stride;
            vbv.SizeInBytes = vertex_buffer_byte_size;

            return vbv;
        }

        D3D12_INDEX_BUFFER_VIEW index_buffer_view() const
        {
            D3D12_INDEX_BUFFER_VIEW ibv;

            ibv.BufferLocation = index_buffer_gpu->GetGPUVirtualAddress();
            ibv.Format = index_format;
            ibv.SizeInBytes = index_buffer_byte_size;

            return ibv;
        }

        // We can free this memory after we finish upload to the GPU.
        void dispose_uploaders()
        {
            vertex_buffer_uploader = nullptr;
            index_buffer_uploader = nullptr;
        }
    };

    struct ObjectConstants
    {
        DirectX::XMFLOAT4X4 world_view_proj = math_helper::Identity4x4();
    };

    struct ReginaContext
    {
        wrl::com_ptr<ID3D12RootSignature> root_signature = nullptr;
        wrl::com_ptr<ID3D12DescriptorHeap> cbv_heap = nullptr;

        rsl::unique_ptr<renderer::directx::UploadBuffer<ObjectConstants>> object_constant_buffer;

        rsl::unique_ptr<Mesh> mesh_cube = nullptr;

        wrl::com_ptr<ID3DBlob> vertex_shader_byte_code;
        wrl::com_ptr<ID3DBlob> pixel_shader_byte_code;

        D3D12_INPUT_ELEMENT_DESC input_layout[2]; // 2 == POSITION & COLOR

        DirectX::XMFLOAT4X4 world = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 view = math_helper::Identity4x4();
        DirectX::XMFLOAT4X4 proj = math_helper::Identity4x4();
    };

    ReginaContext g_regina_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

    ID3D12Device* get_dx12_device()
    {
        return reinterpret_cast<ID3D12Device*>(renderer::backend::get_device());
    }

    ID3D12CommandQueue* get_dx12_command_queue()
    {
        return reinterpret_cast<ID3D12CommandQueue*>(renderer::backend::get_command_queue());
    }

    ID3D12GraphicsCommandList* get_dx12_command_list()
    {
        return reinterpret_cast<ID3D12GraphicsCommandList*>(renderer::backend::get_command_list());
    }

    ID3D12CommandAllocator* get_dx12_command_allocator()
    {
        return reinterpret_cast<ID3D12CommandAllocator*>(renderer::backend::get_command_allocator());
    }

    DXGI_FORMAT get_dx12_backbuffer_format()
    {
        return static_cast<DXGI_FORMAT>(renderer::backend::get_backbuffer_format());
    }

    DXGI_FORMAT get_dx12_depthstencil_format()
    {
        return static_cast<DXGI_FORMAT>(renderer::backend::get_depthstencil_format());
    }

    wrl::com_ptr<ID3DBlob> compile_shader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
    {
        u32 compile_flags = 0;
#if defined(REX_DEBUG)
        compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

        wrl::com_ptr<ID3DBlob> byte_code = nullptr;
        wrl::com_ptr<ID3DBlob> errors = nullptr;

        hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compile_flags, 0, &byte_code, &errors);

        if (errors != nullptr)
        {
            REX_ERROR(LogRegina, "{}", (char*)errors->GetBufferPointer());
            return nullptr;
        }

        if (FAILED(hr))
        {
            REX_ERROR(LogRegina, "Failed to compile shader");
            return nullptr;
        }

        return byte_code;
    }

    bool build_descriptor_heaps()
    {
        D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc;

        cbv_heap_desc.NumDescriptors = 1;
        cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbv_heap_desc.NodeMask = 0; // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )

        if (FAILED(get_dx12_device()->CreateDescriptorHeap(&cbv_heap_desc, IID_PPV_ARGS(&g_regina_ctx.cbv_heap))))
        {
            REX_ERROR(LogRegina, "Failed to create descriptor heap for constant buffer");
            return false;
        }

        return true;
    }

    bool build_constant_buffers()
    {
        g_regina_ctx.object_constant_buffer = rsl::make_unique<renderer::directx::UploadBuffer<ObjectConstants>>(get_dx12_device(), 1, renderer::directx::IsConstantBuffer::yes);

        u32 obj_cb_byte_size = rex::round_up_to_nearest_multiple_of(sizeof(ObjectConstants), 256);

        D3D12_GPU_VIRTUAL_ADDRESS cb_address = g_regina_ctx.object_constant_buffer->resource()->GetGPUVirtualAddress();

        s32 box_constant_buffer_index = 0;
        cb_address += box_constant_buffer_index *= obj_cb_byte_size;

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;

        cbv_desc.BufferLocation = cb_address;
        cbv_desc.SizeInBytes = rex::round_up_to_nearest_multiple_of(sizeof(ObjectConstants), 256);

        get_dx12_device()->CreateConstantBufferView(&cbv_desc, g_regina_ctx.cbv_heap->GetCPUDescriptorHandleForHeapStart());

        return true;
    }

    bool build_root_signature()
    {
        // Shader programs typically require resources as input (constant buffers,
        // textures, samplers). The root signature defines the resources the shader
        // programs expect. If we think of the shader programs as a function, and
        // the input resources as function parameters, then the root signature can be
        // thought of as defining the function signature.

        // Root parameter can be a table, root descriptor or root constants.
        CD3DX12_ROOT_PARAMETER slot_root_parameter[1];

        // Create a single descriptor table of CBVs.
        CD3DX12_DESCRIPTOR_RANGE cbv_table;
        cbv_table.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

        slot_root_parameter[0].InitAsDescriptorTable(1, &cbv_table);

        // A root signature is an array of root parameters.
        CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(1, slot_root_parameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        // Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
        wrl::com_ptr<ID3DBlob> serialized_root_sig = nullptr;
        wrl::com_ptr<ID3DBlob> error_blob = nullptr;

        HRESULT hr = D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, serialized_root_sig.GetAddressOf(), error_blob.GetAddressOf());
        if (error_blob != nullptr)
        {
            REX_ERROR(LogRegina, "{}", (char*)error_blob->GetBufferPointer());
            return false;
        }

        if (FAILED(hr))
        {
            REX_ERROR(LogRegina, "Failed to serialize root signature");
            return false;
        }

        if (FAILED(get_dx12_device()->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&g_regina_ctx.root_signature))))
        {
            REX_ERROR(LogRegina, "Failed to create root signature");
            return false;
        }

        return true;
    }

    bool build_shader_and_input_layout()
    {
        g_regina_ctx.vertex_shader_byte_code = compile_shader(L"shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
        if (g_regina_ctx.vertex_shader_byte_code == nullptr)
        {
            REX_ERROR(LogRegina, "Failed to compile vertex shader");
            return false;
        }
        g_regina_ctx.pixel_shader_byte_code = compile_shader(L"shaders\\color.hlsl", nullptr, "PS", "ps_5_0");
        if (g_regina_ctx.pixel_shader_byte_code == nullptr)
        {
            REX_ERROR(LogRegina, "Failed to compile pixel shader");
            return false;
        }

        g_regina_ctx.input_layout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
        g_regina_ctx.input_layout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

        return true;
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

        if (FAILED(D3DCreateBlob(vb_byte_size, &g_regina_ctx.mesh_cube->vertex_buffer_cpu)))
        {
            REX_ERROR(LogRegina, "Could not create vertex buffer blob");
            return false;
        }
        CopyMemory(g_regina_ctx.mesh_cube->vertex_buffer_cpu->GetBufferPointer(), vertices.data(), vb_byte_size);

        if (FAILED(D3DCreateBlob(ib_byte_size, &g_regina_ctx.mesh_cube->index_buffer_cpu)))
        {
            REX_ERROR(LogRegina, "Could not create index buffer blob");
            return false;
        }
        CopyMemory(g_regina_ctx.mesh_cube->index_buffer_cpu->GetBufferPointer(), indices.data(), ib_byte_size);

        g_regina_ctx.mesh_cube->vertex_buffer_gpu = renderer::directx::create_default_buffer(get_dx12_device(), get_dx12_command_list(), vertices.data(), vb_byte_size, g_regina_ctx.mesh_cube->vertex_buffer_uploader);
        if (g_regina_ctx.mesh_cube->vertex_buffer_gpu == nullptr)
        {
            REX_ERROR(LogRegina, "Could not create GPU vertex buffer");
            return false;
        }
        g_regina_ctx.mesh_cube->index_buffer_gpu = renderer::directx::create_default_buffer(get_dx12_device(), get_dx12_command_list(), indices.data(), ib_byte_size, g_regina_ctx.mesh_cube->index_buffer_uploader);
        if (g_regina_ctx.mesh_cube->index_buffer_gpu == nullptr)
        {
            REX_ERROR(LogRegina, "Could not create GPU index buffer");
            return false;
        }

        g_regina_ctx.mesh_cube->vertex_byte_stride = sizeof(renderer::directx::VertexPosCol);
        g_regina_ctx.mesh_cube->vertex_buffer_byte_size = vb_byte_size;
        g_regina_ctx.mesh_cube->index_format = DXGI_FORMAT_R16_UINT;
        g_regina_ctx.mesh_cube->index_buffer_byte_size = ib_byte_size;

        Submesh submesh;
        submesh.index_count = (UINT)indices.size();
        submesh.start_index_location = 0;
        submesh.base_vertex_location = 0;

        g_regina_ctx.mesh_cube->draw_args["box"] = submesh;

        return true;
    }

    bool build_pipeline_state_object()
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc;
        ZeroMemory(&pso_desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

        pso_desc.InputLayout = { g_regina_ctx.input_layout, (u32)_countof(g_regina_ctx.input_layout) };
        pso_desc.pRootSignature = g_regina_ctx.root_signature.Get();
        pso_desc.VS = { reinterpret_cast<BYTE*>(g_regina_ctx.vertex_shader_byte_code->GetBufferPointer()), g_regina_ctx.vertex_shader_byte_code->GetBufferSize() };
        pso_desc.PS = { reinterpret_cast<BYTE*>(g_regina_ctx.pixel_shader_byte_code->GetBufferPointer()), g_regina_ctx.pixel_shader_byte_code->GetBufferSize() };
        pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        pso_desc.SampleMask = UINT_MAX;
        pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pso_desc.NumRenderTargets = 1;
        pso_desc.RTVFormats[0] = get_dx12_backbuffer_format();
        pso_desc.SampleDesc.Count = renderer::backend::get_msaa_enabled() ? 4 : 1;
        pso_desc.SampleDesc.Quality = renderer::backend::get_msaa_enabled() ? (renderer::backend::get_msaa_quality() - 1) : 0;
        pso_desc.DSVFormat = get_dx12_depthstencil_format();

        return renderer::backend::create_pipeline_state_object(&pso_desc);
    }

    bool initialize()
    {
        REX_LOG(LogRegina, "Initializing Regina");

        // Reset the command list to prep for initialization commands.
        if (FAILED(get_dx12_command_list()->Reset(get_dx12_command_allocator(), nullptr)))
        {
            REX_ERROR(LogRegina, "Unable to reset command list to prep initialization commands");
            return false;
        }

        if (!build_descriptor_heaps()) return false;
        if (!build_constant_buffers()) return false;
        if (!build_root_signature()) return false;
        if (!build_shader_and_input_layout()) return false;
        if (!build_cube_geometry()) return false;
        if (!build_pipeline_state_object()) return false;

        // Execute the initialization commands.
        if (FAILED(get_dx12_command_list()->Close()))
        {
            REX_ERROR(LogRegina, "Unable to close command list");
            return false;
        }

        ID3D12CommandList* cmd_lists[] = { get_dx12_command_list() };
        get_dx12_command_queue()->ExecuteCommandLists(_countof(cmd_lists), cmd_lists);

        // Wait until initialization is complete.
        renderer::backend::flush_command_queue();

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
