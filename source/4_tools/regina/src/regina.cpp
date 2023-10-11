#include "rex_engine/core_application.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/entrypoint.h"
#include "rex_std/string.h"
#include "rex_std_extra/memory/memory_size.h"
#include "rex_windows/gui_application.h"
#include "rex_windows/platform_creation_params.h"

#include "rex_directx/wrl/wrl_types.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/directx_upload_buffer.h"

#include <DirectXMath.h>

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

  struct ObjectConstants
  {
    DirectX::XMFLOAT4X4 world_view_proj = math_helper::Identity4x4();
  };

  struct ReginaContext
  {
    wrl::com_ptr<ID3D12RootSignature> root_signature = nullptr;
    wrl::com_ptr<ID3D12DescriptorHeap> cbv_heap = nullptr;

    rsl::unique_ptr<renderer::directx::UploadBuffer<ObjectConstants>> object_constant_buffer;

    // mesh

    wrl::com_ptr<ID3DBlob> vertex_shader_byte_code;
    wrl::com_ptr<ID3DBlob> pixel_shader_byte_code;

    D3D12_INPUT_ELEMENT_DESC input_layout[2];   // 2 == POSITION & COLOR

    wrl::com_ptr<ID3D12PipelineState> pipeline_state_object;

    DirectX::XMFLOAT4X4 world = math_helper::Identity4x4();
    DirectX::XMFLOAT4X4 view = math_helper::Identity4x4();
    DirectX::XMFLOAT4X4 proj = math_helper::Identity4x4();
  };

  bool initialize()
  {
    REX_LOG(LogRegina, "Initializing Regina");



    return true;
  }
  void update()
  {
    // Nothing to implement
  }
  void draw()
  {
    // Nothing to implement
  }
  void shutdown()
  {
    REX_LOG(LogRegina, "shutting down Regina");
  }

  ApplicationCreationParams create_regina_app_creation_params(PlatformCreationParams&& platformParams)
  {
    ApplicationCreationParams app_params(rsl::move(platformParams));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina";

    app_params.engine_params.max_memory        = 256_kb;
    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_draw_func     = draw;
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
