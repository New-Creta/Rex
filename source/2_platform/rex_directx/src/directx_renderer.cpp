#include "rex_directx/d3dx12.h"
#include "rex_directx/directx_feature_level.h"
#include "rex_directx/directx_feature_shader_model.h"
#include "rex_directx/directx_util.h" // IWYU pragma: keep
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/util.h"
#include "rex_directx/log.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_directx/utility/vertex.h"
#include "rex_directx/resources/clear_state_resource.h"
#include "rex_directx/resources/raster_state_resource.h"
#include "rex_directx/resources/input_layout_resource.h"
#include "rex_directx/resources/pixel_shader_resource.h"
#include "rex_directx/resources/vertex_shader_resource.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/memory/memory_allocation.h"
#include "rex_renderer_core/gpu_description.h"
#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/resource_pool.h"

#include "rex_std/algorithm.h"
#include "rex_std/bonus/string.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"
#include "rex_std_extra/memory/memory_size.h"

#include <Windows.h>
#include <cstddef>
#include <d3d12.h>

#include <DirectXColors.h>
#include <D3Dcompiler.h>

namespace rex
{
    namespace renderer
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            count_t highest_scoring_gpu(const rsl::vector<GpuDescription>& gpus)
            {
                auto it = rsl::max_element(gpus.cbegin(), gpus.cend(),
                    [](const GpuDescription& lhs, const GpuDescription& rhs)
                    {
                        const size_t lhs_vram = lhs.dedicated_video_memory.size_in_bytes();
                        const size_t rhs_vram = rhs.dedicated_video_memory.size_in_bytes();

                        return rhs_vram > lhs_vram;
                    });

                return it != gpus.cend() ? rsl::distance(gpus.cbegin(), it) : -1;
            }
        } // namespace internal

        namespace directx
        {
            Info g_renderer_info; // NOLINT (fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

            //-------------------------------------------------------------------------
            D3D12_FILL_MODE to_d3d12_fill_mode(FillMode mode)
            {
                switch (mode)
                {
                case FillMode::SOLID:
                    return D3D12_FILL_MODE_SOLID;
                case FillMode::WIREFRAME:
                    return D3D12_FILL_MODE_WIREFRAME;
                }

                REX_ASSERT_X(false, "Unsupported fill mode given");
                return D3D12_FILL_MODE_SOLID;
            }
            //-------------------------------------------------------------------------
            D3D12_CULL_MODE to_d3d12_cull_mode(CullMode mode)
            {
                switch (mode)
                {
                case CullMode::NONE:
                    return D3D12_CULL_MODE_NONE;
                case CullMode::FRONT:
                    return D3D12_CULL_MODE_FRONT;
                case CullMode::BACK:
                    return D3D12_CULL_MODE_BACK;
                }

                REX_ASSERT_X(false, "Unsupported cull mode given");
                return D3D12_CULL_MODE_NONE;
            }
            //-------------------------------------------------------------------------
            DXGI_FORMAT to_d3d12_vertex_format(VertexBufferFormat format)
            {
                switch (format)
                {
                case VertexBufferFormat::FLOAT1:
                    return DXGI_FORMAT_R32_FLOAT;
                case VertexBufferFormat::FLOAT2:
                    return DXGI_FORMAT_R32G32_FLOAT;
                case VertexBufferFormat::FLOAT3:
                    return DXGI_FORMAT_R32G32B32_FLOAT;
                case VertexBufferFormat::FLOAT4:
                    return DXGI_FORMAT_R32G32B32A32_FLOAT;
                case VertexBufferFormat::UNORM1:
                    return DXGI_FORMAT_R8_UNORM;
                case VertexBufferFormat::UNORM2:
                    return DXGI_FORMAT_R8G8_UNORM;
                case VertexBufferFormat::UNORM4:
                    return DXGI_FORMAT_R8G8B8A8_UNORM;
                }
                REX_ASSERT_X(false, "Unsupported vertex buffer format given");
                return DXGI_FORMAT_UNKNOWN;
            }
            //-------------------------------------------------------------------------
            DXGI_FORMAT to_d3d12_index_format(IndexBufferFormat format)
            {
                switch (format)
                {
                case IndexBufferFormat::R16_UINT:
                    return DXGI_FORMAT_R16_UINT;
                case IndexBufferFormat::R32_UINT:
                    return DXGI_FORMAT_R32_UINT;
                }
                REX_ASSERT_X(false, "Unsupported index buffer format given");
                return DXGI_FORMAT_UNKNOWN;
            }
            //-------------------------------------------------------------------------
            D3D12_PRIMITIVE_TOPOLOGY to_d3d12_topology(PrimitiveTopology topology)
            {
                switch (topology)
                {
                case PrimitiveTopology::LINELIST:
                    return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                case PrimitiveTopology::LINESTRIP:
                    return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
                case PrimitiveTopology::POINTLIST:
                    return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                case PrimitiveTopology::TRIANGLELIST:
                    return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                case PrimitiveTopology::TRIANGLESTRIP:
                    return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
                }
                REX_ASSERT_X(false, "Unsupported primitive topology given");
                return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
            }

            //-------------------------------------------------------------------------
            char8* heapdesc_type_to_string(D3D12_DESCRIPTOR_HEAP_TYPE type)
            {
                switch (type)
                {
                case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
                    return "D3D12_DESCRIPTOR_HEAP_TYPE_RTV";
                case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
                    return "D3D12_DESCRIPTOR_HEAP_TYPE_DSV";
                case D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
                    return "D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV";
                }

                REX_ASSERT_X(false, "Unsupported descriptor heap type");
                return "D3D12_DESCRIPTOR_HEAP_TYPE_UNKNOWN";
            }

        } // namespace directx

        //-------------------------------------------------------------------------
        const Info& info()
        {
            return directx::g_renderer_info;
        }

        //-------------------------------------------------------------------------
        ShaderPlatform shader_platform()
        {
            return ShaderPlatform::HLSL;
        }

        //-------------------------------------------------------------------------
        bool is_y_up()
        {
            return false;
        }

        //-------------------------------------------------------------------------
        bool is_depth_0_to_1()
        {
            return true;
        }

        namespace backend
        {
            static const u32 s_swapchain_buffer_count = 2;
            static const u32 s_max_color_targets = 8;

            struct DirectXContext
            {
                wrl::com_ptr<ID3D12Device> device = nullptr;
                wrl::com_ptr<ID3D12Fence> fence = nullptr;

                u64 current_fence;

                u32 rtv_desc_size = 0;
                u32 dsv_desc_size = 0;
                u32 cbv_srv_uav_desc_size = 0;

                DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;

                bool msaa_state = false;
                u32 msaa_quality = 0;

                wrl::com_ptr<ID3D12CommandQueue> command_queue = nullptr;
                wrl::com_ptr<ID3D12GraphicsCommandList> command_list = nullptr;
                wrl::com_ptr<ID3D12CommandAllocator> command_allocator = nullptr;

                wrl::com_ptr<IDXGISwapChain> swapchain = nullptr;
                u32 current_back_buffer = 0;

                wrl::com_ptr<ID3D12DescriptorHeap> rtv_heap = nullptr;
                wrl::com_ptr<ID3D12DescriptorHeap> dsv_heap = nullptr;

                wrl::com_ptr<ID3D12Resource> rtv_buffers[s_swapchain_buffer_count];
                DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;

                wrl::com_ptr<ID3D12Resource> depth_stencil_buffer = nullptr;

                rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, wrl::com_ptr<ID3D12DescriptorHeap>> descriptor_heap_pool;

                D3D12_VIEWPORT screen_viewport = {};
                RECT scissor_rect = {};

                wrl::com_ptr<ID3D12PipelineState> pipeline_state_object;

                ResourcePool<rsl::unique_ptr<IResource>> resource_pool;
                
                u32 backbuffer_color;
                u32 backbuffer_depth;
                u32 active_color_targets[s_max_color_targets] = {0};
                u32 active_depth_target;
                u32 num_active_color_targets;
            };

            DirectXContext g_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

            //-------------------------------------------------------------------------
            bool create_command_objects()
            {
                D3D12_COMMAND_QUEUE_DESC queue_desc = {};
                queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
                queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                if (FAILED(g_ctx.device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(g_ctx.command_queue.GetAddressOf()))))
                {
                    REX_ERROR(LogDirectX, "Failed to create command queue");
                    return false;
                }

                if (FAILED(g_ctx.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(g_ctx.command_allocator.GetAddressOf()))))
                {
                    REX_ERROR(LogDirectX, "Failed to create command allocator");
                    return false;
                }

                if (FAILED(g_ctx.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_ctx.command_allocator.Get()
                    , nullptr
                    , IID_PPV_ARGS(g_ctx.command_list.GetAddressOf()))))
                {
                    REX_ERROR(LogDirectX, "Failed to create command list");
                    return false;
                }

                // Start off in a closed state. This is because the first time we
                // refer to the command list we will Reset it, and it needs to be closed
                // before calling Reset.
                g_ctx.command_list->Close();

                return true;
            }

            //-------------------------------------------------------------------------
            bool create_swapchain_object(dxgi::Factory* factory, const OutputWindowUserData& userData)
            {
                g_ctx.swapchain.Reset();

                DXGI_SWAP_CHAIN_DESC sd;
                sd.BufferDesc.Width = userData.window_width;
                sd.BufferDesc.Height = userData.window_height;
                sd.BufferDesc.RefreshRate.Numerator = userData.refresh_rate;
                sd.BufferDesc.RefreshRate.Denominator = 1;
                sd.BufferDesc.Format = g_ctx.back_buffer_format;
                sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
                sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
                sd.SampleDesc.Count = g_ctx.msaa_state ? 4 : 1;
                sd.SampleDesc.Quality = g_ctx.msaa_state ? g_ctx.msaa_quality - 1 : 0;
                sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                sd.BufferCount = s_swapchain_buffer_count;
                sd.OutputWindow = (HWND)userData.primary_display_handle;
                sd.Windowed = userData.windowed;
                sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

                // Note: swap chain uses queue to perform flush.
                rex::wrl::com_ptr<IDXGIFactory> dxgi_factory = factory->as<IDXGIFactory>();
                if (FAILED(dxgi_factory->CreateSwapChain(g_ctx.command_queue.Get(), &sd, g_ctx.swapchain.GetAddressOf())))
                {
                    REX_ERROR(LogDirectX, "Failed to create swap chain");
                    return false;
                }

                return true;
            }

            //-------------------------------------------------------------------------
            bool create_rtvs_and_dsv_descriptor_heaps()
            {
                D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
                rtv_heap_desc.NumDescriptors = s_swapchain_buffer_count;
                rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                rtv_heap_desc.NodeMask = 0;

                if (FAILED(g_ctx.device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(g_ctx.rtv_heap.GetAddressOf()))))
                {
                    REX_ERROR(LogDirectX, "Failed to create RTV heap");
                    return false;
                }

                D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
                dsv_heap_desc.NumDescriptors = 1;
                dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
                dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                dsv_heap_desc.NodeMask = 0;

                if (FAILED(g_ctx.device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(g_ctx.dsv_heap.GetAddressOf()))))
                {
                    REX_ERROR(LogDirectX, "Failed to create DSV heap");
                    return false;
                }

                return true;
            }

            //-------------------------------------------------------------------------
            ID3D12Resource* get_current_backbuffer()
            {
                return g_ctx.rtv_buffers[g_ctx.current_back_buffer].Get();
            }

            //-------------------------------------------------------------------------
            D3D12_CPU_DESCRIPTOR_HANDLE get_current_backbuffer_descriptor()
            {
                return CD3DX12_CPU_DESCRIPTOR_HANDLE(g_ctx.rtv_heap->GetCPUDescriptorHandleForHeapStart(), g_ctx.current_back_buffer, g_ctx.rtv_desc_size);
            }
            
            //-------------------------------------------------------------------------
            D3D12_CPU_DESCRIPTOR_HANDLE get_depthstencil_descriptor()
            {
                return g_ctx.dsv_heap->GetCPUDescriptorHandleForHeapStart();
            }

            //-------------------------------------------------------------------------
            bool create_rtvs_for_swapchain(const OutputWindowUserData& userData, u32 bbColorTargetSlot)
            {
                // Flush before changing any resources.
                flush_command_queue();

                if (FAILED(g_ctx.command_list->Reset(g_ctx.command_allocator.Get(), nullptr)))
                {
                    REX_ERROR(LogDirectX, "Failed to reset command list");
                    return false;
                }

                // Release the previous resources we will be recreating.
                for (int i = 0; i < s_swapchain_buffer_count; ++i)
                {
                    g_ctx.rtv_buffers[i].Reset();
                }

                g_ctx.depth_stencil_buffer.Reset();

                // Resize the swap chain.
                if (FAILED(g_ctx.swapchain->ResizeBuffers(s_swapchain_buffer_count, userData.window_width, userData.window_height, g_ctx.back_buffer_format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)))
                {
                    REX_ERROR(LogDirectX, "Failed to resize swapchain buffers");
                    return false;
                }

                g_ctx.current_back_buffer = 0;

                CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(g_ctx.rtv_heap->GetCPUDescriptorHandleForHeapStart());
                for (u32 i = 0; i < s_swapchain_buffer_count; ++i)
                {
                    if (FAILED(g_ctx.swapchain->GetBuffer(i, IID_PPV_ARGS(g_ctx.rtv_buffers[i].GetAddressOf()))))
                    {
                        REX_ERROR(LogDirectX, "Failed to retrieve swapchain buffer");
                        return false;
                    }

                    // We need to define our own desc struct to enabled SRGB.
                    // We can't initialize the swapchain with 'DXGI_FORMAT_R8G8B8A8_UNORM_SRGB'
                    // so we have to initialize the render targets with this format
                    // and then pass that through to CreateRenderTargetView
                    D3D12_RENDER_TARGET_VIEW_DESC rtv_desc;
                    rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                    rtv_desc.Texture2D.MipSlice = 0;
                    rtv_desc.Texture2D.PlaneSlice = 0;
                    rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                    g_ctx.device->CreateRenderTargetView(g_ctx.rtv_buffers[i].Get(), &rtv_desc, rtv_handle);

                    rtv_handle.Offset(1, g_ctx.rtv_desc_size);
                }

                return true;
            }

            //-------------------------------------------------------------------------
            bool create_dsv_for_swapchain(const OutputWindowUserData& userData, s32 bbDepthTargetSlot)
            {
                D3D12_RESOURCE_DESC resource_tex2d_desc = {};
                resource_tex2d_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                resource_tex2d_desc.Alignment = 0;
                resource_tex2d_desc.Width = userData.window_width;
                resource_tex2d_desc.Height = userData.window_height;
                resource_tex2d_desc.DepthOrArraySize = 1;
                resource_tex2d_desc.MipLevels = 1;

                // SSAO requires an SRV to the depth buffer to read from
                // the depth buffer.  Therefore, because we need to create two views to the same resource:
                //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
                //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
                // we need to create the depth buffer resource with a typeless format.  
                resource_tex2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;

                resource_tex2d_desc.SampleDesc.Count = g_ctx.msaa_state ? 4 : 1;
                resource_tex2d_desc.SampleDesc.Quality = g_ctx.msaa_state ? g_ctx.msaa_quality - 1 : 0;
                resource_tex2d_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
                resource_tex2d_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

                D3D12_CLEAR_VALUE optimized_clear_value = {};
                optimized_clear_value.Format = g_ctx.depth_stencil_format;
                optimized_clear_value.DepthStencil.Depth = 1.0f;
                optimized_clear_value.DepthStencil.Stencil = 0;

                CD3DX12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE_DEFAULT);

                if (FAILED(g_ctx.device->CreateCommittedResource(&heap_properties
                    , D3D12_HEAP_FLAG_NONE
                    , &resource_tex2d_desc
                    , D3D12_RESOURCE_STATE_COMMON
                    , &optimized_clear_value
                    , IID_PPV_ARGS(g_ctx.depth_stencil_buffer.GetAddressOf()))))
                {
                    REX_ERROR(LogDirectX, "Failed to create depth stencil buffer");
                    return false;
                }

                // Create descriptor to mip level 0 of entire resource using the 
                // format of the resource
                D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
                dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
                dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                dsv_desc.Format = g_ctx.depth_stencil_format;
                dsv_desc.Texture2D.MipSlice = 0;

                g_ctx.device->CreateDepthStencilView(g_ctx.depth_stencil_buffer.Get(), &dsv_desc, get_depthstencil_descriptor());

                // Transition the resouce from it's inital state to be used as a depth buffer
                CD3DX12_RESOURCE_BARRIER depth_write_transition = CD3DX12_RESOURCE_BARRIER::Transition(g_ctx.depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
                g_ctx.command_list->ResourceBarrier(1, &depth_write_transition);

                return true;
            }

            //-------------------------------------------------------------------------
            bool create_descriptor_set_pools(u32 numRTV, u32 numDSV, u32 numCBV)
            {
                D3D12_DESCRIPTOR_HEAP_DESC heap_descs[] =
                {
                    {
                        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
                        numRTV,
                        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
                        0    // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )
                    },
                    {
                        D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
                        numDSV,
                        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
                        0    // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )
                    },
                    {
                        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                        numCBV,
                        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
                        0    // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )
                    }
                };

                for (s32 i = 0; i < _countof(heap_descs); ++i)
                {
                    auto heap_desc = &heap_descs[i];
                    auto heap = g_ctx.descriptor_heap_pool[heap_desc->Type];

                    if (FAILED(g_ctx.device->CreateDescriptorHeap(heap_desc, IID_PPV_ARGS(&heap))))
                    {
                        REX_ERROR(LogDirectX, "Failed to create descriptor heap for type: {}", directx::heapdesc_type_to_string(heap_desc->Type));
                        return false;
                    }
                }

                return true;
            }

            //-------------------------------------------------------------------------
            void* get_device()
            {
                return g_ctx.device.Get();
            }
            
            //-------------------------------------------------------------------------
            void* get_command_queue()
            {
                return g_ctx.command_queue.Get();
            }
            
            //-------------------------------------------------------------------------
            void* get_command_list()
            {
                return g_ctx.command_list.Get();
            }
            
            //-------------------------------------------------------------------------
            void* get_command_allocator()
            {
                return g_ctx.command_allocator.Get();
            }

            //-------------------------------------------------------------------------
            s32 get_backbuffer_format()
            {
                return static_cast<s32>(g_ctx.back_buffer_format);
            }
            
            //-------------------------------------------------------------------------
            s32 get_depthstencil_format()
            {
                return static_cast<s32>(g_ctx.depth_stencil_format);
            }

            //-------------------------------------------------------------------------
            bool get_msaa_enabled()
            {
                return g_ctx.msaa_state;
            }
            
            //-------------------------------------------------------------------------
            s32 get_msaa_quality()
            {
                return g_ctx.msaa_quality;
            }

            //------------------------------------------------------------------------
            bool create_pipeline_state_object(void* psoDescription)
            {
                D3D12_GRAPHICS_PIPELINE_STATE_DESC* pso_desc = reinterpret_cast<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>(psoDescription);

                if (FAILED(g_ctx.device->CreateGraphicsPipelineState(pso_desc, IID_PPV_ARGS(&g_ctx.pipeline_state_object))))
                {
                    REX_ERROR(LogDirectX, "Failed to create pipeline state object");
                    return false;
                }

                return true;
            }

            //-------------------------------------------------------------------------
            bool flush_command_queue()
            {
                // Advance the fence value to mark commands up to this fence point.
                g_ctx.current_fence++;

                // Add an instruction to the command queue to set a new fence point. Because we
                // are on the GPU timeline, the new fence point won't be set until the GPU finishes
                // processing all the commands prior to this Signal().
                if (FAILED(g_ctx.command_queue->Signal(g_ctx.fence.Get(), g_ctx.current_fence)))
                {
                    REX_ERROR(LogDirectX, "Failed to signal command queue with fence");
                    return false;
                }

                if (g_ctx.fence->GetCompletedValue() < g_ctx.current_fence)
                {
                    HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

                    //Fire event when GPU hits current fence
                    if (FAILED(g_ctx.fence->SetEventOnCompletion(g_ctx.current_fence, event_handle)))
                    {
                        REX_ERROR(LogDirectX, "Failed to set completion event for fence");
                        return false;
                    }

                    WaitForSingleObject(event_handle, INFINITE);
                    CloseHandle(event_handle);
                }

                return true;
            }

            //-------------------------------------------------------------------------
            bool create_clear_state(const parameters::ClearState& cs, u32 resourceSlot)
            {
                resources::ClearState rcs;

                rcs.rgba = cs.rgba;
                rcs.depth = cs.depth;
                rcs.stencil = cs.stencil;               
                rcs.flags = cs.flags;

                g_ctx.resource_pool.validate_and_grow_if_necessary(resourceSlot);
                g_ctx.resource_pool[resourceSlot] = rsl::make_unique<ClearStateResource>(rcs);

                return true;
            }
            //-------------------------------------------------------------------------
            bool create_raster_state(const parameters::RasterState& rs, u32 resourceSlot)
            {
                D3D12_RASTERIZER_DESC d3d_rs;

                d3d_rs.FillMode = directx::to_d3d12_fill_mode(rs.fill_mode);
                d3d_rs.CullMode = directx::to_d3d12_cull_mode(rs.cull_mode);
                d3d_rs.FrontCounterClockwise = rs.front_ccw;
                d3d_rs.DepthBias = rs.depth_bias;
                d3d_rs.DepthBiasClamp = rs.depth_bias_clamp;
                d3d_rs.SlopeScaledDepthBias = rs.sloped_scale_depth_bias;
                d3d_rs.DepthClipEnable = rs.depth_clip_enable;
                d3d_rs.ForcedSampleCount = rs.forced_sample_count;

                /**
                * Conservative rasterization means that all pixels that are at least partially covered by a rendered primitive are rasterized, which means that the pixel shader is invoked. 
                * Normal behavior is sampling, which is not used if conservative rasterization is enabled.
                * 
                * Conservative rasterization is useful in a number of situations outside of rendering (collision detection, occlusion culling, and visibility detection).
                * 
                * https://learn.microsoft.com/en-us/windows/win32/direct3d11/conservative-rasterization
                */
                d3d_rs.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
                d3d_rs.MultisampleEnable = rs.multisample;
                d3d_rs.AntialiasedLineEnable = rs.aa_lines;

                g_ctx.resource_pool.validate_and_grow_if_necessary(resourceSlot);
                g_ctx.resource_pool[resourceSlot] = rsl::make_unique<RasterStateResource>(d3d_rs);

                return true;
            }
            //-------------------------------------------------------------------------
            bool create_input_layout(const parameters::CreateInputLayout& cil, u32 resourceSlot)
            {
                rsl::vector<D3D12_INPUT_ELEMENT_DESC> input_element_descriptions(rsl::Capacity(cil.num_elements));

                for (s32 i = 0; i < cil.num_elements; ++i)
                {
                    input_element_descriptions[i].SemanticName = cil.input_layout[i].semantic_name;
                    input_element_descriptions[i].SemanticIndex = cil.input_layout[i].semantic_index;
                    input_element_descriptions[i].Format = directx::to_d3d12_vertex_format(cil.input_layout[i].format);
                    input_element_descriptions[i].InputSlot = cil.input_layout[i].input_slot;
                    input_element_descriptions[i].AlignedByteOffset = cil.input_layout[i].aligned_byte_offset;
                    input_element_descriptions[i].InputSlotClass = cil.input_layout[i].input_slot_class 
                        ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA 
                        : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    input_element_descriptions[i].InstanceDataStepRate = cil.input_layout[i].instance_data_step_rate;
                }

                g_ctx.resource_pool.validate_and_grow_if_necessary(resourceSlot);
                g_ctx.resource_pool[resourceSlot] = rsl::make_unique<InputLayoutResource>(input_element_descriptions);

                return true;
            }
            //-------------------------------------------------------------------------
            bool create_buffer(const parameters::CreateBuffer& cb, u32 resourceSlot)
            {

            }

            //-------------------------------------------------------------------------
            bool load_shader(const parameters::LoadShader& ls, u32 resourceSlot)
            {
                wrl::com_ptr<ID3DBlob> byte_code;
                if (FAILED(D3DCreateBlob(ls.byte_code_size, byte_code.GetAddressOf())))
                {
                    REX_ERROR(LogDirectX, "Failed to load shader");
                    return false;
                }

                // Copy the data into the ID3DBlob's memory.
                memcpy(byte_code->GetBufferPointer(), ls.byte_code, ls.byte_code_size);

                g_ctx.resource_pool.validate_and_grow_if_necessary(resourceSlot);

                switch (ls.shader_type)
                {
                case ShaderType::VERTEX:
                    g_ctx.resource_pool[resourceSlot] = rsl::make_unique<VertexShaderResource>(byte_code);
                    break;
                case ShaderType::PIXEL:
                    g_ctx.resource_pool[resourceSlot] = rsl::make_unique<PixelShaderResource>(byte_code);
                    break;

                default:
                    REX_ERROR(LogDirectX, "Unsupported Shader Type was given");
                    return false;
                }

                return true;
            }
            //-------------------------------------------------------------------------
            bool link_shader(const parameters::LinkShader& /*ls*/, u32 /*resourceSlot*/)
            {
                // Nothing to do on this platform
            }
            //-------------------------------------------------------------------------
            bool compile_shader(const parameters::CompileShader& cs, u32 resourceSlot)
            {
                u32 compile_flags = 0;
#if defined(REX_DEBUG)
                compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

                HRESULT hr = S_OK;

                wrl::com_ptr<ID3DBlob> byte_code = nullptr;
                wrl::com_ptr<ID3DBlob> errors = nullptr;

                hr = D3DCompile2(cs.shader_code, cs.shader_code_size, cs.shader_name.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, cs.shader_entry_point, cs.shader_feature_target, compile_flags, 0, 0, 0, 0, &byte_code, &errors);

                if (errors != nullptr)
                {
                    REX_ERROR(LogDirectX, "{}", (char*)errors->GetBufferPointer());
                    return false;
                }

                if (FAILED(hr))
                {
                    REX_ERROR(LogDirectX, "Failed to compile shader");
                    return false;
                }

                g_ctx.resource_pool.validate_and_grow_if_necessary(resourceSlot);

                switch (cs.shader_type)
                {
                case ShaderType::VERTEX:
                    g_ctx.resource_pool[resourceSlot] = rsl::make_unique<VertexShaderResource>(byte_code);
                    break;
                case ShaderType::PIXEL:
                    g_ctx.resource_pool[resourceSlot] = rsl::make_unique<PixelShaderResource>(byte_code);
                    break;

                default:
                    REX_ERROR(LogDirectX, "Unsupported Shader Type was given");
                    return false;
                }                

                return true;
            }

            //-------------------------------------------------------------------------
            void clear(u32 clearStateIndex)
            {
                const ClearStateResource& csr = get_resource_from_pool_as<ClearStateResource>(g_ctx.resource_pool, clearStateIndex);

                ClearBits flags = csr.get()->flags;

                REX_ASSERT_X(flags != ClearBits::NONE, "No clear flags given but renderer::backend::clear was called.");

                if (flags & ClearBits::CLEAR_COLOR_BUFFER)
                {
                    D3D12_CPU_DESCRIPTOR_HANDLE backbuffer_desc = get_current_backbuffer_descriptor();

                    g_ctx.command_list->ClearRenderTargetView(backbuffer_desc, csr.get()->rgba.getData(), 0, nullptr);
                }

                if (flags & ClearBits::CLEAR_DEPTH_BUFFER || flags & ClearBits::CLEAR_STENCIL_BUFFER)
                {
                    D3D12_CPU_DESCRIPTOR_HANDLE depthstencil_desc = get_depthstencil_descriptor();

                    u32 depth_stencil_clear_flags = 0;
                    depth_stencil_clear_flags |= flags & ClearBits::CLEAR_DEPTH_BUFFER ? D3D12_CLEAR_FLAG_DEPTH : 0;
                    depth_stencil_clear_flags |= flags & ClearBits::CLEAR_STENCIL_BUFFER ? D3D12_CLEAR_FLAG_STENCIL : 0;

                    g_ctx.command_list->ClearDepthStencilView(depthstencil_desc, (D3D12_CLEAR_FLAGS)depth_stencil_clear_flags, csr.get()->depth, csr.get()->stencil, 0, nullptr);
                }
            }

            //-------------------------------------------------------------------------
            bool release_resource(u32 resourceSlot)
            {
            }

            //-------------------------------------------------------------------------
            bool initialize(const OutputWindowUserData& userData, u32 bbColorTargetSlot, u32 bbDepthTargetSlot)
            {
#if defined(REX_DEBUG)
                // Enable extra debuggin and send debug messages to the VC++ output window
                rex::wrl::com_ptr<ID3D12Debug> debug_controller;
                if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
                {
                    REX_ERROR(LogDirectX, "Failed to create DX debug controller");
                    return false;
                }

                debug_controller->EnableDebugLayer();
#endif

                dxgi::Factory factory = dxgi::Factory::create();
                if (!factory)
                {
                    REX_ERROR(LogDirectX, "Failed to create DXGI Factory");
                    return false;
                }

                const dxgi::AdapterManager adapter_manager(&factory, &internal::highest_scoring_gpu);
                const dxgi::Adapter* selected_gpu = adapter_manager.selected();
                IDXGIAdapter* adapter = selected_gpu->c_ptr();

                const D3D_FEATURE_LEVEL feature_level = query_feature_level(adapter);

                if (FAILED(D3D12CreateDevice(adapter, static_cast<D3D_FEATURE_LEVEL>(feature_level), IID_PPV_ARGS(&g_ctx.device))))
                {
                    REX_ERROR(LogDirectX, "Software adapter not supported");
                    REX_ERROR(LogDirectX, "Failed to create DX12 Device");
                    return false;
                }

                REX_LOG(LogDirectX, "D3D12 Device Created!");

                const D3D_SHADER_MODEL shader_model = query_shader_model_version(g_ctx.device.Get());

                directx::g_renderer_info.shader_version = rsl::string(shader_model_name(shader_model));
                directx::g_renderer_info.api_version = rsl::string(feature_level_name(feature_level));
                directx::g_renderer_info.adaptor = selected_gpu->description().name;
                directx::g_renderer_info.vendor = selected_gpu->description().vendor_name;

                // Create fence for CPU/GPU synchronization
                if (FAILED(g_ctx.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_ctx.fence))))
                {
                    REX_ERROR(LogDirectX, "Failed to create DX fence, to synchronize CPU/GPU");
                    return false;
                }

                // Descriptor sizes vary across GPU so we need to query this information
                g_ctx.rtv_desc_size = g_ctx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                g_ctx.dsv_desc_size = g_ctx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                g_ctx.cbv_srv_uav_desc_size = g_ctx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

                // 4x MSAA is supported on all DX11 hardware, since we only support DX12 devices we are guaranteed that 4x MSAA is supported.
                // However, we do have to check the supported quality level.
                D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ms_quality_levels;
                ms_quality_levels.Format = g_ctx.back_buffer_format;
                ms_quality_levels.SampleCount = 4;
                ms_quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
                ms_quality_levels.NumQualityLevels = 0;
                if (FAILED(g_ctx.device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &ms_quality_levels, sizeof(ms_quality_levels))))
                {
                    REX_ERROR(LogDirectX, "Given multisample quality level is not supported by this hardware");
                    return false;
                }

                g_ctx.msaa_quality = ms_quality_levels.NumQualityLevels;

                REX_ASSERT_X(g_ctx.msaa_quality > 0, "Unexcpected MSAA quality level");

                // Create command queue, command allocator and command list objects
                if (create_command_objects() == false)
                {
                    REX_ERROR(LogDirectX, "Failed to create command objects");
                    return false;
                }

                // Create swapchain object
                if (create_swapchain_object(&factory, userData) == false)
                {
                    REX_ERROR(LogDirectX, "Failed to create swapchain object");
                    return false;
                }

                // Create an initial set of descriptors
                // Create descriptor heaps for storing the rtv and dsv.
                // We need "swapchain_buffer_count" amount of rtvs to describe the buffer resources within the swapchain
                //  and one dsv to describe the depth/stencil buffer resource for depth testing.
                if (create_descriptor_set_pools(s_swapchain_buffer_count, 1, 32) == false)
                {
                    REX_ERROR(LogDirectX, "Failed to create descriptor set pools");
                    return false;
                }

                // Create rtvs for each render target within the swapchain
                if (create_rtvs_for_swapchain(userData, bbColorTargetSlot) == false)
                {
                    REX_ERROR(LogDirectX, "Failed to create rtvs for swapchain");
                    return false;
                }

                // Create depth/stencil view to bind to the pipeline and
                // depth/stencil buffer as resource to write depth values to
                if (create_dsv_for_swapchain(userData, bbDepthTargetSlot) == false)
                {
                    REX_ERROR(LogDirectX, "Failed to create dsv");
                    return false;
                }

                // Done recording commands!
                if (FAILED(g_ctx.command_list->Close()))
                {
                    REX_ERROR(LogDirectX, "Failed to close command list");
                    return false;
                }

                // Add the command list to the queue for execution
                ID3D12CommandList* command_lists[] = { g_ctx.command_list.Get() };
                g_ctx.command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

                // Wait until resize is complete.
                flush_command_queue();

                // Create viewport to render our image in
                // A viewport always needs to reset whenever a command list is reset
                g_ctx.screen_viewport.TopLeftX = 0.0f;
                g_ctx.screen_viewport.TopLeftY = 0.0f;
                g_ctx.screen_viewport.Width = static_cast<f32>(userData.window_width);
                g_ctx.screen_viewport.Height = static_cast<f32>(userData.window_height);
                g_ctx.screen_viewport.MinDepth = 0.0f;
                g_ctx.screen_viewport.MaxDepth = 1.0f;

                // Cull pixels drawn outside of the backbuffer ( such as UI elements )
                g_ctx.scissor_rect = { 0, 0, static_cast<s32>(userData.window_width), static_cast<s32>(userData.window_height) };

                return true;
            }

            //-------------------------------------------------------------------------
            void shutdown()
            {
                if (g_ctx.device != nullptr)
                {
                    flush_command_queue();
                }
            }

            //-------------------------------------------------------------------------
            void draw(u32 vertexCount, u32 startVertex, PrimitiveTopology topology)
            {
                REX_ASSERT_X(false, "renderer::draw is unsupported when using DX12, use renderer::draw_indexed_instanced or renderer::draw_instanced");
            }
            //-------------------------------------------------------------------------
            void draw_indexed(u32 indexCount, u32 startIndex, u32 baseVertex, PrimitiveTopology topology)
            {
                REX_ASSERT_X(false, "renderer::draw is unsupported when using DX12, use renderer::draw_indexed_instanced or renderer::draw_instanced");
            }
            //-------------------------------------------------------------------------
            void draw_indexed_instanced(u32 instanceCount, u32 startInstance, u32 indexCount, u32 startIndex, u32 baseVertex, PrimitiveTopology topology)
            {
                g_ctx.command_list->IASetPrimitiveTopology(directx::to_d3d12_topology(topology));
                g_ctx.command_list->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
            }
            void draw_instanced(u32 vertexCount, u32 instanceCount, u32 startVertex, u32 startInstance, PrimitiveTopology topology)
            {
                g_ctx.command_list->IASetPrimitiveTopology(directx::to_d3d12_topology(topology));
                g_ctx.command_list->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
            }

            //-------------------------------------------------------------------------
            bool set_render_targets(const u32* const colorTargets, u32 numColorTargets, u32 depthTarget, u32 color_slice = 0, u32 depth_slice = 0)
            {
                mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
            }
            //-------------------------------------------------------------------------
            bool set_input_layout(u32 inputLayoutSlot)
            {

            }
            //-------------------------------------------------------------------------
            bool set_viewport(const Viewport& viewport)
            {
                g_ctx.command_list->RSSetViewports(1, (D3D12_VIEWPORT*)&viewport)
            }
            //-------------------------------------------------------------------------
            bool set_scissor_rect(const ScissorRect& rect)
            {
                g_ctx.command_list->RSSetScissorRects(1, (D3D12_RECT*)&rect);
            }
            //-------------------------------------------------------------------------
            bool set_vertex_buffers(u32* bufferIndices, u32 numBuffers, u32 startSlot, const u32* strides, const u32* offsets)
            {

            }
            //-------------------------------------------------------------------------
            bool set_index_buffer(u32 bufferIndex, IndexBufferFormat format, u32 offset)
            {

            }
            //-------------------------------------------------------------------------
            bool set_shader(u32 shaderIndex, ShaderType shaderType)
            {

            }
            //-------------------------------------------------------------------------
            bool set_raster_state(u32 rasterStateIndex)
            {

            }

            //-------------------------------------------------------------------------
            bool new_frame()
            {
                // Reuse the memory assosiated with command recording.
                // We can only reset when the associated command lists have finished
                // execution on the GPU.
                if (FAILED(g_ctx.command_allocator->Reset()))
                {
                    REX_ERROR(LogDirectX, "Failed to reset command allocator");
                    return false;
                }

                // a command list can be reset after it has been added to the command queue via ExecuteCommandList. Reusing the 
                // command list reuses memory.
                if (FAILED(g_ctx.command_list->Reset(g_ctx.command_allocator.Get(), g_ctx.pipeline_state_object.Get())))
                {
                    REX_ERROR(LogDirectX, "Failed to reset command list");
                    return false;
                }

                // Set the viewport and scissor rect. This needs to be reset
                // whenever the command list is reset.
                g_ctx.command_list->RSSetViewports(1, &g_ctx.screen_viewport);
                g_ctx.command_list->RSSetScissorRects(1, &g_ctx.scissor_rect);

                // Indicate a state transition on the resouce usage.
                D3D12_RESOURCE_BARRIER render_target_transition = CD3DX12_RESOURCE_BARRIER::Transition(get_current_backbuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

                g_ctx.command_list->ResourceBarrier(1, &render_target_transition);

                return true;
            }

            //-------------------------------------------------------------------------
            bool end_frame()
            {
                // Wait until frame commands are complete. This waiting is 
                // inefficient and is done for simplicity.
                flush_command_queue();

                return true;
            }

            //-------------------------------------------------------------------------
            bool present()
            {
                // Indicate a state transition on the resouce usage.
                D3D12_RESOURCE_BARRIER present_transition = CD3DX12_RESOURCE_BARRIER::Transition(get_current_backbuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
                g_ctx.command_list->ResourceBarrier(1, &present_transition);

                // Done recording commands!
                if (FAILED(g_ctx.command_list->Close()))
                {
                    REX_ERROR(LogDirectX, "Failed to close command list");
                    return false;
                }

                // Add the command list to the queue for execution
                ID3D12CommandList* command_lists[] = { g_ctx.command_list.Get() };
                g_ctx.command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

                // Swap the back and front buffers
                if (FAILED(g_ctx.swapchain->Present(0, 0)))
                {
                    REX_ERROR(LogDirectX, "Failed to present swapchain");
                    return false;
                }

                g_ctx.current_back_buffer = (g_ctx.current_back_buffer + 1) % s_swapchain_buffer_count;

                return true;
            }
        } // namespace backend
    }   // namespace renderer
} // namespace rex