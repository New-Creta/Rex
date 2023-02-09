#pragma once

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/core_application.h"
#include "rex_engine/defines.h"
#include "rex_std/memory.h"
#include "rex_windows/win_types.h"

namespace rex
{
  struct FrameInfo;
  struct ApplicationCreationParams;
  struct PlatformCreationParams;

  struct ApplicationCreationParams
  {
    s32 window_width          = 1280;
    s32 window_height         = 720;
    const char8* window_title = "Application";

    s32 max_render_commands = 1 << 16;
    s32 max_fps             = 60;
  };

  namespace win32
  {
    class GuiApplication : public IApplication
    {
    public:
      GuiApplication(const PlatformCreationParams& platformParams, const ApplicationCreationParams& appParams, CommandLineArguments&& cmdArgs);
      GuiApplication(const GuiApplication&) = delete;
      GuiApplication(GuiApplication&&)      = delete;
      ~GuiApplication() override;

      GuiApplication& operator=(const GuiApplication&) = delete;
      GuiApplication& operator=(GuiApplication&&)      = delete;

      bool is_running() const override;

      s32 run() override;
      void quit() override;

    protected:
      void mark_for_destroy();

      virtual bool app_initialize()
      {
        return true;
      };
      virtual void app_update(const FrameInfo& info)
      {
        UNUSED_PARAM(info);
      };
      virtual void app_shutdown() {};

    private:
      struct Internal;
      rsl::unique_ptr<Internal> m_internal_ptr;
    };
  } // namespace win32
} // namespace rex