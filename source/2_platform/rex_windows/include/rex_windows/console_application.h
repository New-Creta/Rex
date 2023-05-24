#pragma once

#include "rex_engine/core_application.h"
#include "rex_std/utility.h"
#include "rex_windows/platform_creation_params.h"

namespace rex
{
  namespace win32
  {
    class ConsoleApplication : public CoreApplication
    {
    public:
      explicit ConsoleApplication(ApplicationCreationParams&& appParams);
      ConsoleApplication(const ConsoleApplication&) = delete;
      ConsoleApplication(ConsoleApplication&&)      = delete;
      ~ConsoleApplication() override;

      ConsoleApplication& operator=(const ConsoleApplication&) = delete;
      ConsoleApplication& operator=(ConsoleApplication&&)      = delete;

    private:
      bool platform_init() override;
      void platform_update() override;
      void platform_shutdown() override;

    private:
      class Internal;
      rsl::unique_ptr<Internal> m_internal;
    };
  } // namespace win32
} // namespace rex