#pragma once

#include "rex_engine/core_application.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_std/utility.h"

namespace rex
{
  namespace win32
  {
    class ConsoleApplication : public CoreApplication
    {
    public:
      explicit ConsoleApplication(ApplicationCreationParams&& creationParams);
      ConsoleApplication(const ConsoleApplication&) = delete;
      ConsoleApplication(ConsoleApplication&&) = delete;
      ~ConsoleApplication();

      ConsoleApplication& operator=(const ConsoleApplication&) = delete;
      ConsoleApplication& operator=(ConsoleApplication&&) = delete;

    private:
      bool platform_init() override;
      void platform_update() override;
      void platform_shutdown() override;

    private:
      class Internal;
      static constexpr card32 s_internal_size = 264;
      static constexpr card32 s_internal_alignment = 8;
      rsl::aligned_storage<s_internal_size, s_internal_alignment> m_internal_obj;
      Internal* m_internal;
    };
  }
}