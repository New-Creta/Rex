#pragma once

#include "rex_engine/app/core_application.h"
#include "rex_engine/engine/types.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"
#include "rex_windows/engine/platform_creation_params.h"

namespace rex
{
  namespace win
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
  } // namespace win
} // namespace rex