#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/globals.h"

#include "rex_engine/memory/memory_types.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/functional.h"

namespace rex
{
  class SettingsManager
  {
  public:
    // Check if a certain setting exists
    bool has_setting(rsl::string_view name) const;

    // Get the value of a setting as a string
    rsl::string_view get_string(rsl::string_view name, rsl::string_view defaultVal = "") const;
    // Get the value of a setting as an int
    s32 get_int(rsl::string_view name, s32 defaultVal = 0) const;
    // Get the value of a setting as a float
    f32 get_float(rsl::string_view name, f32 defaultVal = 0.0f) const;
    // Get the value of a setting as a bool
    bool get_bool(rsl::string_view name, bool defaultVal = false) const;

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

    // Load a settings file or directory and adds all settings files found to the settings
    void load(rsl::string_view path);

    // Load a directory containing settings files and add all of them to the settings
    void load_directory(rsl::string_view path);

    // Load a settings file and adds it settings to the settings
    // This behaves the same as if you can "set" multiple times
    // for each setting in the file
    void load_file(rsl::string_view path);


    // unload all settings
    void unload();

  private:
    scratch_string to_hash_key(rsl::string_view header, rsl::string_view key) const;
    void add_new_settings(rsl::string_view header, rsl::string_view key, rsl::string_view val);
    rsl::optional<rsl::string_view> get_setting(rsl::string_view name) const;

  private:
    using hash_map = rsl::unordered_map<
      rsl::string,
      rsl::string,
      rsl::hash_lower<rsl::string>,
      rsl::equal_to_case_insensitive<rsl::string>>;

    hash_map m_all_settings;
  };


  namespace settings
  {
    void init(globals::GlobalUniquePtr<SettingsManager> settingsManager);
    SettingsManager* instance();
    void shutdown();
  } // namespace settings
} // namespace rex