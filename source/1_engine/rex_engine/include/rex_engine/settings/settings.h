#pragma once

#include "rex_std/string_view.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace settings
  {
    // Check if a certain setting exists
    bool has_setting(rsl::string_view name);

    // Get the value of a setting as a string
    rsl::string_view get(rsl::string_view name);

    // Set a setting. This supports adding new settings
    void set(rsl::string_view name, rsl::string_view val);

    // Load a settings file and adds it settings to the settings
    // This behaves the same as if you can "set" multiple times
    // for each setting in the file
    void load(rsl::string_view path);
  }
}