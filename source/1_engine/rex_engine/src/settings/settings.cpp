#include "rex_engine/settings/settings.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/text_processing/ini.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_std/algorithm.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace settings
  {
    DEFINE_LOG_CATEGORY(LogSettings);

    namespace internal
    {
      // This maps holds all the settings
      // it gets filled up on boot but can always be changed at runtime
      rsl::unordered_map<rsl::string, rsl::string>& all_settings()
      {
        static rsl::unordered_map<rsl::string, rsl::string> all_settings_map;
        return all_settings_map;
      }

      // add a new setting to the internal map
      // setting headers and keys are not allows to have spaces
      void add_new_settings(rsl::string_view header, rsl::string_view key, rsl::string_view val)
      {
        // If a setting comes from a header, we add the header to the final name
        // and seperate it by a '.'
        rsl::string full_setting_name;
        if(!header.empty())
        {
          full_setting_name.assign(rsl::format("{}.{}", header, key));
        }
        else
        {
          full_setting_name.assign(key);
        }

        // remove all the spaces from a setting's name before we add it to the list
        remove_spaces(full_setting_name);

        rsl::to_lower(full_setting_name.data(), full_setting_name.data(), full_setting_name.length());
        all_settings()[rsl::move(full_setting_name)] = rsl::string(val);
      }

      rsl::optional<rsl::string_view> get_setting(rsl::string_view name)
      {
        TempString name_lower(name.length(), '\0');
        rsl::to_lower(name.data(), name_lower.data(), name.length());
        rsl::string_view name_lower_view = name_lower;
        if (all_settings().contains(name_lower_view))
        {
          return all_settings().at(name_lower_view);
        }
        return rsl::nullopt;
      }
    } // namespace internal

    // Check if a certain setting exists
    bool has_setting(rsl::string_view name)
    {
      return internal::get_setting(name).has_value();
    }

    // gets a setting from the global map
    rsl::string_view get_string(rsl::string_view name, rsl::string_view defaultVal)
    {
      return internal::get_setting(name).value_or(defaultVal);
    }

    // Get the value of a setting as an int
    s32 get_int(rsl::string_view name, s32 defaultVal)
    {
      if (has_setting(name))
      {
        return rsl::stoi(get_string(name)).value_or(defaultVal);
      }

      return defaultVal;
    }
    // Get the value of a setting as a float
    f32 get_float(rsl::string_view name, f32 defaultVal)
    {
      if (has_setting(name))
      {
        return rsl::stof(get_string(name)).value_or(defaultVal);
      }

      return defaultVal;
    }
    bool get_bool(rsl::string_view name, bool defaultVal)
    {
      if (has_setting(name))
      {
        return rsl::stob(get_string(name)).value_or(defaultVal);
      }

      return defaultVal;
    }

    // set s asetting in the global map
    void set(rsl::string_view name, rsl::string_view val)
    {
      rsl::string name_lower(name);
      rsl::to_lower(name_lower.cbegin(), name_lower.begin(), name_lower.size());

      internal::all_settings()[name_lower].assign(val);
    }

    // Set a setting from an int. This supports adding new settings
    void set(rsl::string_view name, s32 val)
    {
      rsl::string name_lower(name);
      rsl::to_lower(name_lower.cbegin(), name_lower.begin(), name_lower.size());

      internal::all_settings()[name_lower].assign(rsl::to_string(val));
    }
    // Set a setting from a float. This supports adding new settings
    void set(rsl::string_view name, f32 val)
    {
      rsl::string name_lower(name);
      rsl::to_lower(name_lower.cbegin(), name_lower.begin(), name_lower.size());

      internal::all_settings()[name_lower].assign(rsl::to_string(val));
    }

    // Load a settings file and adds it settings to the settings
    // This behaves the same as if you can "set" multiple times
    // for each setting in the file
    void load(rsl::string_view path)
    {
      // of course if the path doesn't exist, we exit early
      if(!file::exists(path))
      {
        REX_ERROR(LogSettings, "Cannot load settings, file doesn't exist. File: {}", path);
        return;
      }

      // LogSettings are just plain ini files
      // so we can use the ini processor here
      rex::ini::Ini ini_content = rex::ini::read_from_file(path);
      if (ini_content.is_discarded())
      {
        REX_ERROR(LogSettings, "Cannot read settings file {}", rex::path::abs_path(path));
        REX_ERROR(LogSettings, ini_content.parse_error().error_msg());
      }

      // Loop over the processed settings and add them to the global map
      rsl::vector<ini::IniBlock> blocks = ini_content.all_blocks();

      for(const ini::IniBlock& block : ini_content.all_blocks())
      {
        for (const auto[key, value] : block.all_items())
        {
          internal::add_new_settings(block.header(), key, value);
        }
      }
    }

    // unload all settings
    void unload()
    {
      internal::all_settings().clear();
    }
  } // namespace settings
} // namespace rex