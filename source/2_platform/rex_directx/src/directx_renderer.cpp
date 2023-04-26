#include "rex_directx/directx_feature_level.h"
#include "rex_directx/directx_feature_shader_model.h"
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/log.h"
#include "rex_renderer_core/highest_vram_gpu_scorer.h"
#include "rex_renderer_core/renderer.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace renderer
  {
    namespace directx
    {
      RendererInfo g_renderer_info; // NOLINT (fuchsia-statically-constructed-objects,-warnings-as-errors, cppcoreguidelines-avoid-non-const-global-variables,-warnings-as-errors)
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
      struct directx_context
      {
        wrl::com_ptr<ID3D12Device> device = nullptr;
      };

      directx_context g_ctx;

      //-------------------------------------------------------------------------
      bool initialize()
      {
        rsl::unique_ptr<dxgi::Factory> factory = dxgi::Factory::create();

        dxgi::AdapterManager adapter_manager(factory.get(), HighestVramGpuScorer());
        const dxgi::Adapter* adapter = static_cast<const dxgi::Adapter*>(adapter_manager.selected());

        FeatureLevelInfo feature_level_info = query_feature_level(adapter->c_ptr());

        if(FAILED(D3D12CreateDevice(adapter->c_ptr(), (D3D_FEATURE_LEVEL)feature_level_info.level, IID_PPV_ARGS(&g_ctx.device))))
        {
          if(adapter_manager.load_software_adapter(factory.get()))
          {
            REX_ERROR(LogDirectX, "Could not create software adapter");
            REX_ERROR(LogDirectX, "Failed to create DX12 Device");
            return false;
          }
          else
          {
            const dxgi::Adapter* software_adapter = static_cast<const dxgi::Adapter*>(adapter_manager.software());
            if(FAILED(D3D12CreateDevice(software_adapter->c_ptr(), (D3D_FEATURE_LEVEL)feature_level_info.level, IID_PPV_ARGS(&g_ctx.device))))
            {
              REX_ERROR(LogDirectX, "Failed to create DX12 Device");
              return false;
            }
          }
        }
        else
        {
          REX_LOG(LogDirectX, "D3D12 Device Created!");
        }

        ShaderModelInfo shader_model_info = query_shader_model_version(g_ctx.device.Get());

        directx::g_renderer_info.shader_version = shader_model_info.name;
        directx::g_renderer_info.api_version    = feature_level_info.name;
        directx::g_renderer_info.adaptor        = adapter->description().name;
        directx::g_renderer_info.vendor         = adapter->description().vendor_name;

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
      void clear() {}

      //-------------------------------------------------------------------------
      void present() {}
    } // namespace backend
  }   // namespace renderer
} // namespace rex