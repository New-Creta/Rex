#pragma once

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/core_application.h"
#include "rex_engine/defines.h"
#include "rex_std/memory.h"
#include "rex_std/utility.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_windows/win_types.h"

namespace rex
{
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