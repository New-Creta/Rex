#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace settings
  {
    // Check if a certain setting exists
    bool has_setting(rsl::string_view name);

    // Get the value of a setting as a string
    rsl::string_view get_string(rsl::string_view name, rsl::string_view defaultVal = "");
    // Get the value of a setting as an int
    s32 get_int(rsl::string_view name, s32 defaultVal = 0);
    // Get the value of a setting as a float
    f32 get_float(rsl::string_view name, f32 defaultVal = 0.0f);
    // Get the value of a setting as a bool
    bool get_bool(rsl::string_view name, bool defaultVal = false);

    // Set a setting from a string. This supports adding new settings
    void set(rsl::string_view name, rsl::string_view val);
    // Set a setting from an int. This supports adding new settings
    void set(rsl::string_view name, s32 val);
    // Set a setting from a float. This supports adding new settings
    void set(rsl::string_view name, f32 val);
    // No setter for bool is provided
    // because it'd be used when using a string literal
    // without it, the string_view overload is used
    // and for bools the integer one is used
    
    // Load a settings file and adds it settings to the settings
    // This behaves the same as if you can "set" multiple times
    // for each setting in the file
    void load(rsl::string_view path);

    // unload all settings
    void unload();

  } // namespace settings
} // namespace rex