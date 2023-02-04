#include "rex_windows/gui_application.h"
#include "rex_windows/win_types.h"

#include "rex_engine/entrypoint.h"
#include "rex_engine/core_application_params.h"
#include "rex_engine/diagnostics/logging.h"

#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>
#include <stdlib.h>

#include <iostream>

namespace rex
{
  //-------------------------------------------------------------------------
  struct command_line_arguments
  {
    command_line_arguments()
      : count(0), values(nullptr)
    {
    }
    command_line_arguments(const command_line_arguments& other)
      : count(0), values(nullptr)
    {
      if (other.count != 0)
      {
        count = other.count;
        values = new char8 * [other.count];
        std::memcpy(values, other.values, other.count);
      }
    }
    command_line_arguments(command_line_arguments&& other) noexcept
      : count(std::exchange(other.count, 0)), values(std::exchange(other.values, nullptr))
    {
    }
    ~command_line_arguments()
    {
      if (values != nullptr)
      {
        for (s32 i = 0; i < count; ++i)
        {
          delete values[i];
        }

        delete[] values;
      }
    }

    command_line_arguments& operator=(const command_line_arguments& other)
    {
      if (other.count != 0)
      {
        count = other.count;
        values = new char8 * [other.count];
        std::memcpy(values, other.values, other.count);
      }
      else
      {
        count = 0;
        values = nullptr;
      }

      return *this;
    }
    command_line_arguments& operator=(command_line_arguments&& other) noexcept
    {
      count = std::exchange(other.count, 0);
      values = std::exchange(other.values, nullptr);

      return *this;
    }

    s32 count;
    char8** values;
  };

  //-------------------------------------------------------------------------
  command_line_arguments get_command_line_arguments()
  {
    s32 argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    command_line_arguments arguments;
    arguments.count = argc;
    arguments.values = new char8 * [argc];

    for (s32 i = 0; i < argc; ++i)
    {
      arguments.values[i] = new char[256];

      size_t num_converted = 0;
      wcstombs_s(&num_converted, arguments.values[i], 256, argv[i], 256);
    }

    LocalFree(argv);

    return arguments;
  }
}

//-------------------------------------------------------------------------
INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  rex::command_line_arguments arguments = rex::get_command_line_arguments();

  rex::ApplicationCreationParams acp = rex::app_entry(arguments.count, arguments.values);

  rex::win32::GuiApplication application((rex::win32::HInstance)hInstance, (rex::win32::HInstance)hPrevInstance, (rex::win32::LPtStr)lpCmdLine, nCmdShow, std::move(acp));

  s32 result = application.run();

  REX_INFO("Application completed with result: {0}", result);

  return result;
}

int main()
{
  WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW);
}
