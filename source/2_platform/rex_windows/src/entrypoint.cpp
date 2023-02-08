#include "rex_engine/entrypoint.h"

#include "rex_engine/core_application_params.h"
#include "rex_engine/diagnostics/logging.h"
#include "rex_windows/gui_application.h"
#include "rex_windows/win_types.h"

#define NOMINMAX
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <shellapi.h>

namespace rex
{
  //-------------------------------------------------------------------------
  struct CommandLineArguments
  {
    CommandLineArguments()
        : count(0)
        , values(nullptr)
    {
    }
    CommandLineArguments(const CommandLineArguments& other)
        : count(0)
        , values(nullptr)
    {
      if(other.count != 0)
      {
        count  = other.count;
        values = new char8*[other.count];
        std::memcpy(values, other.values, other.count);
      }
    }
    CommandLineArguments(CommandLineArguments&& other) noexcept
        : count(std::exchange(other.count, 0))
        , values(std::exchange(other.values, nullptr))
    {
    }
    ~CommandLineArguments()
    {
      if(values != nullptr)
      {
        for(s32 i = 0; i < count; ++i)
        {
          delete values[i];
        }

        delete[] values;
      }
    }

    CommandLineArguments& operator=(const CommandLineArguments& other) // NOLINT(bugprone-unhandled-self-assignment)
    {
      REX_ASSERT_X(this != &other, "self assignment in copy assignment");

      if(other.count != 0)
      {
        count  = other.count;
        values = new char8*[other.count];
        std::memcpy(values, other.values, other.count);
      }
      else
      {
        count  = 0;
        values = nullptr;
      }

      return *this;
    }
    CommandLineArguments& operator=(CommandLineArguments&& other) noexcept
    {
      REX_ASSERT_X(this != &other, "self assignment in move assignment");

      count  = std::exchange(other.count, 0);
      values = std::exchange(other.values, nullptr);

      return *this;
    }

    s32 count;
    char8** values;
  };

  //-------------------------------------------------------------------------
  CommandLineArguments get_command_line_arguments()
  {
    s32 argc     = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    CommandLineArguments arguments;
    arguments.count  = argc;
    arguments.values = new char8*[argc];

    for(s32 i = 0; i < argc; ++i)
    {
      arguments.values[i] = new char[256];

      size_t num_converted = 0;
      (void)wcstombs_s(&num_converted, arguments.values[i], 256, argv[i], 256); // casting to void to silence clang-tidy warning
    }

    LocalFree(argv);

    return arguments;
  }
} // namespace rex

//-------------------------------------------------------------------------
INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
  const rex::CommandLineArguments arguments = rex::get_command_line_arguments();

  rex::ApplicationCreationParams acp = rex::app_entry(arguments.count, arguments.values);

  rex::win32::HInstance hinstance      = static_cast<rex::win32::HInstance>(hInstance);
  rex::win32::HInstance hprev_instance = static_cast<rex::win32::HInstance>(hPrevInstance);
  rex::win32::LPtStr cmd_line          = static_cast<rex::win32::LPtStr>(lpCmdLine);
  rex::win32::GuiApplication application(hinstance, hprev_instance, cmd_line, nShowCmd, rsl::move(acp));

  s32 result = application.run();

  REX_INFO("Application completed with result: {0}", result);

  return result;
}

int main()
{
  return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLine(), SW_SHOW);
}
