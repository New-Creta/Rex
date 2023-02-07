#pragma once

#include "rex_engine/core_application.h"
#include "rex_engine/defines.h"
#include "rex_std/memory.h"
#include "rex_windows/win_types.h"

namespace rex
{
  struct FrameInfo;
  struct ApplicationCreationParams;

  namespace win32
  {
    class GuiApplication : public IApplication
    {
    public:
      GuiApplication(HInstance hInstance, HInstance hPrevInstance, LPtStr lpCmdLine, s32 nCmdShow, ApplicationCreationParams&& creationParams);
      ~GuiApplication() override;

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