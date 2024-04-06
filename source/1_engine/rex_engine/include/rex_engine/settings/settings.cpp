#include "rex_engine/settings/settings.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/text_processing/ini_processor.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_std/algorithm.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace settings
  {
    DEFINE_LOG_CATEGORY(Settings);

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
    } // namespace internal

    // Check if a certain setting exists
    bool has_setting(rsl::string_view name)
    {
      return internal::all_settings().contains(name);
    }

    // gets a setting from the global map
    rsl::string_view get(rsl::string_view name)
    {
      return internal::all_settings()[name];
    }

    // set s asetting in the global map
    void set(rsl::string_view name, rsl::string_view val)
    {
      internal::all_settings()[name].assign(val);
    }

    // Load a settings file and adds it settings to the settings
    // This behaves the same as if you can "set" multiple times
    // for each setting in the file
    void load(rsl::string_view path)
    {
      // of course if the path doesn't exist, we exit early
      if(!file::exists(path))
      {
        return;
      }

      // load the content of the settings file into memory
      rex::memory::Blob settings_blob = vfs::read_file(path);

      // Settings are just plain ini files
      // so we can use the ini processor here
      IniProcessor ini_processor = IniProcessor(memory::BlobView(settings_blob));
      Error error                = ini_processor.process();

      REX_ERROR_X(Settings, !error, "Invalid settings found in \"{}\"", path);

      // Loop over the processed settings and add them to the global map
      for(const IniHeaderWithItems& header_with_items: ini_processor.items())
      {
        for(const rsl::key_value<rsl::string_view, rsl::string_view>& item: header_with_items.items())
        {
          internal::add_new_settings(header_with_items.header(), item.key, item.value);
        }
      }
    }
  } // namespace settings
} // namespace rex