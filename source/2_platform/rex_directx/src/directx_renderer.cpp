#include "rex_directx/directx_feature_level.h"
#include "rex_directx/directx_feature_shader_model.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/log.h"
#include "rex_renderer_core/renderer.h"
#include "rex_std/memory.h"
#include "rex_std_extra/utility/enum_reflection.h"

namespace rex
{
  namespace renderer
  {
    namespace internal
    {
      //-------------------------------------------------------------------------
      const Gpu* highest_scoring_gpu(const rsl::vector<rsl::unique_ptr<Gpu>>& gpus)
      {
        auto it = rsl::max_element(gpus.cbegin(), gpus.cend(),
                                   [](const rsl::unique_ptr<Gpu>& lhs, const rsl::unique_ptr<Gpu>& rhs)
                                   {
                                     const size_t lhs_vram = lhs->description().dedicated_video_memory.size_in_bytes();
                                     const size_t rhs_vram = rhs->description().dedicated_video_memory.size_in_bytes();

                                     return rhs_vram > lhs_vram;
                                   });

        return it != gpus.cend() ? it->get() : nullptr;
      }
    } // namespace internal

    namespace directx
    {
      RendererInfo g_renderer_info; // NOLINT (fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
    }                               // namespace directx

    //-------------------------------------------------------------------------
    const RendererInfo& info()
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
      struct DirectXContext
      {
        wrl::com_ptr<ID3D12Device> device = nullptr;
      };

      DirectXContext g_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

      //-------------------------------------------------------------------------
      bool initialize()
      {
        dxgi::Factory factory = dxgi::Factory::create();
        if(!factory)
        {
          REX_ERROR(LogDirectX, "Failed to create DXGI Factory");
          return false;
        }

        const dxgi::AdapterManager adapter_manager(&factory, &internal::highest_scoring_gpu);
        const Gpu* selected_gpu = adapter_manager.selected();
        IDXGIAdapter* adapter = static_cast<IDXGIAdapter*>(selected_gpu->native_handle());

        const D3D_FEATURE_LEVEL feature_level = query_feature_level(adapter);

        if(FAILED(D3D12CreateDevice(adapter, static_cast<D3D_FEATURE_LEVEL>(feature_level), IID_PPV_ARGS(&g_ctx.device))))
        {
          REX_ERROR(LogDirectX, "Software adapter not supported");
          REX_ERROR(LogDirectX, "Failed to create DX12 Device");
          return false;
        }

        REX_LOG(LogDirectX, "D3D12 Device Created!");

        const D3D_SHADER_MODEL shader_model = query_shader_model_version(g_ctx.device.Get());

        directx::g_renderer_info.shader_version = rsl::string(shader_model_name(shader_model));
        directx::g_renderer_info.api_version    = rsl::string(feature_level_name(feature_level));
        directx::g_renderer_info.adaptor        = selected_gpu->description().name;
        directx::g_renderer_info.vendor         = selected_gpu->description().vendor_name;

        return true;
      }

      //-------------------------------------------------------------------------
      void shutdown()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      void new_frame()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      void end_frame()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      void clear()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      void present()
      {
        // Nothing to implement
      }
    } // namespace backend
  }   // namespace renderer
} // namespace rex