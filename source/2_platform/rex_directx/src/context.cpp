#include "pic_engine_directx12_pch.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "dx_context.h"
#include "dx_helper.h"

#include <stdlib.h>

#define LOG_ADAPTERS 1
#define LOG_ADAPTER_OUTPUTS 0
#define LOG_ADAPTER_OUTPUT_DISPLAY_MODE 0

namespace rex
{
    namespace directx
    {
        //-------------------------------------------------------------------------
        DirectXContext::DirectXContext(const WindowHandle& hwnd)
            :CoreGraphicsContext()
            ,m_handle(hwnd)
        {
#if PIC_DEBUG
            // Enable the D3D12 debug layer.
            {
                Microsoft::WRL::ComPtr<ID3D12Debug> debug_controller;
                DX_CALL(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)));
                debug_controller->EnableDebugLayer();
            }
#endif

            DX_CALL(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgi_factory)));

            // Try to create hardware device.
            HRESULT hardwareResult = D3D12CreateDevice
            (
                nullptr, // default adapter
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&m_d3d_device)
            );

            // Fall back to WARP device.
            if (FAILED(hardwareResult))
            {
                Microsoft::WRL::ComPtr<IDXGIAdapter> p_warp_adapter;
                DX_CALL(m_dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(&p_warp_adapter)));
                DX_CALL(D3D12CreateDevice(p_warp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d_device)));
            }

            DX_CALL(m_d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

            m_rtv_descriptor_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            m_dsv_descriptor_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
            m_cbv_srv_uav_descriptor_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

            // Check 4X MSAA quality support for our back buffer format.
            // All Direct3D 11 capable devices support 4X MSAA for all render 
            // target formats, so we only need to check quality support.

            D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
            msQualityLevels.Format = m_back_buffer_format;
            msQualityLevels.SampleCount = 4;
            msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
            msQualityLevels.NumQualityLevels = 0;
            DX_CALL(m_d3d_device->CheckFeatureSupport(
                D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
                &msQualityLevels,
                sizeof(msQualityLevels)));

            m_4x_msaa_quality = msQualityLevels.NumQualityLevels;

            S_ASSERT_X(m_4x_msaa_quality > 0, "Unexpected MSAA quality level.");

#if defined PIC_DEBUG && LOG_ADAPTERS
            log_adapters();
#endif

            create_command_objects();
            create_swap_chain();
            create_rtv_and_dsv_descriptor_heaps();
        }
        //-------------------------------------------------------------------------
        DirectXContext::~DirectXContext()
        {
            if (m_d3d_device != nullptr)
            {
                flush_command_queue();
            }
        }

        //-------------------------------------------------------------------------
        void DirectXContext::resize(int32 x, int32 y, int32 width, int32 height)
        {
            S_ASSERT(m_d3d_device);
            S_ASSERT(m_swap_chain);
            S_ASSERT(m_direct_cmd_list_alloc);

            // Flush before changing any resources.
            flush_command_queue();

            DX_CALL(m_command_list->Reset(m_direct_cmd_list_alloc.Get(), nullptr));

            // Release the previous resources we will be recreating.
            for (int i = 0; i < s_swap_chain_buffer_count; ++i)
                m_swap_chain_buffers[i].Reset();
            m_depth_stencil_buffer.Reset();

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
            for (UINT i = 0; i < s_swap_chain_buffer_count; i++)
            {
                DX_CALL(m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_swap_chain_buffers[i])));
                m_d3d_device->CreateRenderTargetView(m_swap_chain_buffers[i].Get(), nullptr, rtv_heap_handle);
                rtv_heap_handle.Offset(1, m_rtv_descriptor_size);
            }

            // Create the depth/stencil buffer and view.
            D3D12_RESOURCE_DESC depthStencilDesc;
            depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            depthStencilDesc.Alignment = 0;
            depthStencilDesc.Width = width;
            depthStencilDesc.Height = height;
            depthStencilDesc.DepthOrArraySize = 1;
            depthStencilDesc.MipLevels = 1;

            // Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
            // the depth buffer.  Therefore, because we need to create two views to the same resource:
            //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
            //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
            // we need to create the depth buffer resource with a typeless format.  
            depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

            depthStencilDesc.SampleDesc.Count = m_4x_msaa_state ? 4 : 1;
            depthStencilDesc.SampleDesc.Quality = m_4x_msaa_state ? (m_4x_msaa_quality - 1) : 0;
            depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            CD3DX12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE_DEFAULT);

            D3D12_CLEAR_VALUE optClear;
            optClear.Format = m_depth_stencil_format;
            optClear.DepthStencil.Depth = 1.0f;
            optClear.DepthStencil.Stencil = 0;
            DX_CALL(m_d3d_device->CreateCommittedResource(
                &heap_properties,
                D3D12_HEAP_FLAG_NONE,
                &depthStencilDesc,
                D3D12_RESOURCE_STATE_COMMON,
                &optClear,
                IID_PPV_ARGS(m_depth_stencil_buffer.GetAddressOf())));

            // Create descriptor to mip level 0 of entire resource using the format of the resource.
            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Format = m_depth_stencil_format;
            dsvDesc.Texture2D.MipSlice = 0;
            m_d3d_device->CreateDepthStencilView(m_depth_stencil_buffer.Get(), &dsvDesc, m_dsv_heap->GetCPUDescriptorHandleForHeapStart());

            // Transition the resource from its initial state to be used as a depth buffer.
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
            m_command_list->ResourceBarrier(1, &barrier);

            // Execute the resize commands.
            DX_CALL(m_command_list->Close());
            ID3D12CommandList* cmdsLists[] = { m_command_list.Get() };
            m_command_queue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

            // Wait until resize is complete.
            flush_command_queue();

            // Update the viewport transform to cover the client area.
            m_screen_viewport.TopLeftX = static_cast<float>(x);
            m_screen_viewport.TopLeftY = static_cast<float>(y);
            m_screen_viewport.Width = static_cast<float>(width);
            m_screen_viewport.Height = static_cast<float>(height);
            m_screen_viewport.MinDepth = 0.0f;
            m_screen_viewport.MaxDepth = 1.0f;

            m_scissor_rect = { x, y, width, height };
        }
        //-------------------------------------------------------------------------
        void DirectXContext::make_current()
        {
            // Nothing to implement
        }
        //-------------------------------------------------------------------------
        void DirectXContext::present()
        {
            // Add the command list to the queue for execution.
            ID3D12CommandList* cmdsLists[] = { m_command_list.Get() };
            m_command_queue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

            // swap the back and front buffers
            DX_CALL(m_swap_chain->Present(0, 0));
            m_current_back_buffer_idx = (m_current_back_buffer_idx + 1) % s_swap_chain_buffer_count;

            // Wait until frame commands are complete.  This waiting is inefficient and is
            // done for simplicity.  Later we will show how to organize our rendering code
            // so we do not have to wait per frame.
            flush_command_queue();
        }

        //-------------------------------------------------------------------------
        ID3D12CommandQueue* DirectXContext::get_command_queue() const
        {
            return m_command_queue.Get();
        }
        //-------------------------------------------------------------------------
        ID3D12CommandAllocator* DirectXContext::get_command_list_allocator() const
        {
            return m_direct_cmd_list_alloc.Get();
        }
        //-------------------------------------------------------------------------
        ID3D12GraphicsCommandList* DirectXContext::get_command_list() const
        {
            return m_command_list.Get();
        }

        //-------------------------------------------------------------------------
        const D3D12_VIEWPORT& DirectXContext::get_screen_viewport() const
        {
            return m_screen_viewport;
        }
        //-------------------------------------------------------------------------
        const D3D12_RECT& DirectXContext::get_scissor_rect() const
        {
            return m_scissor_rect;
        }

        //-------------------------------------------------------------------------
        ID3D12Resource* DirectXContext::get_current_back_buffer() const
        {
            return m_swap_chain_buffers[m_current_back_buffer_idx].Get();
        }
        //-------------------------------------------------------------------------
        D3D12_CPU_DESCRIPTOR_HANDLE DirectXContext::get_current_back_buffer_view() const
        {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(
                m_rtv_heap->GetCPUDescriptorHandleForHeapStart(),
                m_current_back_buffer_idx,
                m_rtv_descriptor_size);
        }
        //-------------------------------------------------------------------------
        D3D12_CPU_DESCRIPTOR_HANDLE DirectXContext::get_depth_stencil_view() const
        {
            return m_dsv_heap->GetCPUDescriptorHandleForHeapStart();
        }

        //-------------------------------------------------------------------------
        void DirectXContext::flush_command_queue()
        {
            // Advance the fence value to mark commands up to this fence point.
            m_current_fence++;

            // Add an instruction to the command queue to set a new fence point.  Because we 
            // are on the GPU timeline, the new fence point won't be set until the GPU finishes
            // processing all the commands prior to this Signal().
            DX_CALL(m_command_queue->Signal(m_fence.Get(), m_current_fence));

            // Wait until the GPU has completed commands up to this fence point.
            if (m_fence->GetCompletedValue() < m_current_fence)
            {
                HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

                // Fire event when GPU hits current fence.  
                DX_CALL(m_fence->SetEventOnCompletion(m_current_fence, eventHandle));

                // Wait until the GPU hits current fence event is fired.
                WaitForSingleObject(eventHandle, INFINITE);
                CloseHandle(eventHandle);
            }
        }

        //-------------------------------------------------------------------------
        void DirectXContext::create_command_objects()
        {
            D3D12_COMMAND_QUEUE_DESC queueDesc = {};
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            DX_CALL(m_d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_command_queue)));

            DX_CALL(m_d3d_device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(m_direct_cmd_list_alloc.GetAddressOf())));

            DX_CALL(m_d3d_device->CreateCommandList(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                m_direct_cmd_list_alloc.Get(), // Associated command allocator
                nullptr,                   // Initial PipelineStateObject
                IID_PPV_ARGS(m_command_list.GetAddressOf())));

            // Start off in a closed state.  This is because the first time we refer 
            // to the command list we will Reset it, and it needs to be closed before
            // calling Reset.
            m_command_list->Close();
        }
        //-------------------------------------------------------------------------
        void DirectXContext::create_swap_chain()
        {
            int32 wnd_width = 800;
            int32 wnd_height = 600;

            //RECT rect;
            //if (GetWindowRect(m_handle.as_win32_handle, &rect))
            //{
            //    wnd_width = rect.right - rect.left;
            //    wnd_height = rect.bottom - rect.top;
            //}

            // Release the previous swapchain we will be recreating.
            m_swap_chain.Reset();

            DXGI_SWAP_CHAIN_DESC sd;
            sd.BufferDesc.Width = wnd_width;
            sd.BufferDesc.Height = wnd_height;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferDesc.Format = m_back_buffer_format;
            sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
            sd.SampleDesc.Count = m_4x_msaa_state ? 4 : 1;
            sd.SampleDesc.Quality = m_4x_msaa_state ? (m_4x_msaa_quality - 1) : 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = s_swap_chain_buffer_count;
            sd.OutputWindow = m_handle.as_win32_handle;
            sd.Windowed = true;
            sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            // Note: Swap chain uses queue to perform flush.
            DX_CALL(m_dxgi_factory->CreateSwapChain(m_command_queue.Get(),&sd,m_swap_chain.GetAddressOf()));
        }
        //-------------------------------------------------------------------------
        void DirectXContext::create_rtv_and_dsv_descriptor_heaps()
        {
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
            rtvHeapDesc.NumDescriptors = s_swap_chain_buffer_count;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            rtvHeapDesc.NodeMask = 0;
            
            DX_CALL(m_d3d_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtv_heap.GetAddressOf())));

            D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
            dsvHeapDesc.NumDescriptors = 1;
            dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            dsvHeapDesc.NodeMask = 0;

            DX_CALL(m_d3d_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsv_heap.GetAddressOf())));
        }

        //-------------------------------------------------------------------------
        void DirectXContext::log_adapters()
        {
            UINT i = 0;
            IDXGIAdapter* adapter = nullptr;
            std::vector<IDXGIAdapter*> adapterList;
            while (m_dxgi_factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
            {
                DXGI_ADAPTER_DESC desc;
                adapter->GetDesc(&desc);

                size_t num_converted;
                char s_description[128];
                wcstombs_s(&num_converted, s_description, (size_t)128u, desc.Description, (size_t)128u);

                std::string text = "***Adapter: ";
                text += s_description;

                S_INFO(text.c_str());

                adapterList.push_back(adapter);

                ++i;
            }

            for (size_t j = 0; j < adapterList.size(); ++j)
            {
#if LOG_ADAPTER_OUTPUTS
                log_adapter_outputs(adapterList[j]);
#endif
                RELEASE_COM(adapterList[j]);
            }
        }
        //-------------------------------------------------------------------------
        void DirectXContext::log_adapter_outputs(IDXGIAdapter* adapter)
        {
            UINT i = 0;
            IDXGIOutput* output = nullptr;
            while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
            {
                DXGI_OUTPUT_DESC desc;
                output->GetDesc(&desc);

                size_t num_converted;
                char s_device_name[32];
                wcstombs_s(&num_converted, s_device_name, (size_t)32u, desc.DeviceName, (size_t)32u);

                std::string text = "***Output: ";
                text += s_device_name;

                S_INFO(text.c_str());

#if LOG_ADAPTER_OUTPUT_DISPLAY_MODE
                log_output_display_modes(output, m_back_buffer_format);
#endif

                RELEASE_COM(output);

                ++i;
            }
        }
        //-------------------------------------------------------------------------
        void DirectXContext::log_output_display_modes(IDXGIOutput* output, DXGI_FORMAT format)
        {
            UINT count = 0;
            UINT flags = 0;

            // Call with nullptr to get list count.
            output->GetDisplayModeList(format, flags, &count, nullptr);

            std::vector<DXGI_MODE_DESC> modeList(count);
            output->GetDisplayModeList(format, flags, &count, &modeList[0]);

            for (auto& x : modeList)
            {
                UINT n = x.RefreshRate.Numerator;
                UINT d = x.RefreshRate.Denominator;
                std::string text =
                    "Width = " + std::to_string(x.Width) + " " +
                    "Height = " + std::to_string(x.Height) + " " +
                    "Refresh = " + std::to_string(n) + "/" + std::to_string(d);

                S_INFO(text.c_str());
            }
        }
    }
}