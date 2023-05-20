#pragma once

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/core_application.h"
#include "rex_std/memory.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_engine/types.h"
#include "rex_std/vector.h"

namespace rex
{
  struct GuiParams
  {
    s32 window_width              = 1280;
    s32 window_height             = 720;
    rsl::string_view window_title = "Application";

    s32 max_render_commands = 1 << 16;
    s32 max_fps             = 60;
  };

  struct ApplicationCreationParams
  {
  public:
    ApplicationCreationParams(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs)
        : engine_params()
        , platform_params(rsl::move(platformParams))
        , gui_params()
        , cmd_args(rsl::move(cmdArgs))
    {
    }

  public:
    EngineParams engine_params;
    PlatformCreationParams platform_params;
    GuiParams gui_params;
    CommandLineArguments cmd_args;
  };

  namespace win32
  {
    class GuiApplication : public CoreApplication
    {
    public:
      explicit GuiApplication(ApplicationCreationParams&& appParams);
      GuiApplication(const GuiApplication&) = delete;
      GuiApplication(GuiApplication&&)      = delete;
      ~GuiApplication() override;

      GuiApplication& operator=(const GuiApplication&) = delete;
      GuiApplication& operator=(GuiApplication&&)      = delete;

    private:
      bool platform_init() override;
      void platform_update() override;
      void platform_shutdown() override;

    private:
      class Internal;
      rsl::unique_ptr<Internal> m_internal_ptr;
    };
  } // namespace win32
} // namespace rex