#pragma once

#include "core_context.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>

// Link necessary d3d12 libraries.
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace rex
{
    class Window;

    namespace directx
    {
        struct WindowHandle
        {
            union
            {
                void* as_void_ptr;
                HWND as_win32_handle;
            };
        };

        class DirectXContext : public CoreGraphicsContext
        {
        public:
            DirectXContext(const WindowHandle& hwnd);
            ~DirectXContext() override;

            void resize(int32 x, int32 y, int32 width, int32 height) override;
            void make_current() override;
            void present() override;

            ID3D12CommandQueue* get_command_queue() const;
            ID3D12CommandAllocator* get_command_list_allocator() const;
            ID3D12GraphicsCommandList* get_command_list() const;

            const D3D12_VIEWPORT& get_screen_viewport() const;
            const D3D12_RECT& get_scissor_rect() const;

            ID3D12Resource* get_current_back_buffer() const;
            D3D12_CPU_DESCRIPTOR_HANDLE get_current_back_buffer_view() const;
            D3D12_CPU_DESCRIPTOR_HANDLE get_depth_stencil_view() const;

        private:
            void log_adapters();
            void log_adapter_outputs(IDXGIAdapter* adapter);
            void log_output_display_modes(IDXGIOutput* output, DXGI_FORMAT format);

            void flush_command_queue();

            void create_command_objects();
            void create_swap_chain();
            void create_rtv_and_dsv_descriptor_heaps();

            WindowHandle m_handle;

            bool m_4x_msaa_state = false;
            uint32 m_4x_msaa_quality = 0;

            Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgi_factory;
            Microsoft::WRL::ComPtr<ID3D12Device> m_d3d_device;
            Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain;

            Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
            uint64 m_current_fence;

            Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_command_queue;
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_direct_cmd_list_alloc;
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;

            static const int32 s_swap_chain_buffer_count = 2;
            int32 m_current_back_buffer_idx = 0;
            Microsoft::WRL::ComPtr<ID3D12Resource> m_swap_chain_buffers[s_swap_chain_buffer_count];
            Microsoft::WRL::ComPtr<ID3D12Resource> m_depth_stencil_buffer;

            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsv_heap;

            D3D12_VIEWPORT m_screen_viewport;
            D3D12_RECT m_scissor_rect;

            uint32 m_rtv_descriptor_size = 0;
            uint32 m_dsv_descriptor_size = 0;
            uint32 m_cbv_srv_uav_descriptor_size = 0;

            D3D_DRIVER_TYPE m_d3d_driver_type = D3D_DRIVER_TYPE_HARDWARE;
            DXGI_FORMAT m_back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
            DXGI_FORMAT m_depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        };
    }
}