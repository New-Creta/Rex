#include "rex_windows_pch.h"

#include "display_manager.h"
#include "display_mode.h"
#include "display.h"

namespace rex
{
    //-------------------------------------------------------------------------
    DisplayManager::DisplayManager()
        : m_active_display(nullptr)
    {
        // int32 available_video_displays = SDL_GetNumVideoDisplays();
        // if (available_video_displays < 1)
        // {
        //     R_ERROR("Failed to retrieve available video displays");
        //     return;
        // }
        // 
        // R_INFO("[APPLICATION] Number of available video displays: {0}", available_video_displays);
        // 
        // for (int32 i = 0; i < available_video_displays; ++i)
        // {
        //     int32 available_display_modes = SDL_GetNumDisplayModes(i);
        //     if (available_display_modes < 1)
        //     {
        //         R_ERROR("Failed to retrieve available display modes for display with index: {0}", i);
        //         continue;
        //     }
        // 
        //     const char* display_name = SDL_GetDisplayName(i);
        // 
        //     R_INFO("\t{0}", display_name);
        //     R_TRACE("\t{0} of available display modes for display with name: {1}", available_display_modes, display_name);
        // 
        //     DisplayDescription display_description;
        //     display_description.display_index = i;
        //     display_description.name = display_name;
        // 
        //     SDL_DisplayMode mode;
        //     for (int32 j = 0; j < available_display_modes; ++j)
        //     {
        //         if (SDL_GetDisplayMode(i, j, &mode))
        //         {
        //             R_ERROR("Failed to retrieve display mode from display with index: {0} and mode with index: {1}", i, j);
        //             continue;
        //         }
        // 
        //         R_TRACE("[DisplayMode,DisplayMode][{0},{1}]: [BPP, FMT, W, H, RR][{2}, {3}, {4}, {5}, {6}]", display_name, j, SDL_BITSPERPIXEL(mode.format), SDL_GetPixelFormatName(mode.format), mode.w, mode.h, mode.refresh_rate);
        // 
        //         DisplayModeDescription display_mode_description;
        //         display_mode_description.mode_index = j;
        //         display_mode_description.bits_per_pixel = SDL_BITSPERPIXEL(mode.format);
        //         display_mode_description.pixel_format = mode.format;
        //         display_mode_description.width = mode.w;
        //         display_mode_description.height = mode.h;
        //         display_mode_description.refresh_rate = mode.refresh_rate;
        // 
        //         display_description.display_modes.insert(std::make_pair(j, DisplayMode(display_mode_description)));
        //     }
        // 
        //     m_displays.insert(std::make_pair(i, Display(display_description)));
        // }

        DisplayDescription display_description;
        display_description.display_index = 0;
        display_description.name = "Simulated Display Description";

        DisplayModeDescription display_mode_description;
        display_mode_description.mode_index = 0;
        display_mode_description.bits_per_pixel = 24;
        display_mode_description.width = 1280;
        display_mode_description.height = 720;
        display_mode_description.refresh_rate = 60;

        display_description.display_modes.insert(std::make_pair(0, DisplayMode(display_mode_description)));

        m_displays.insert(std::make_pair(0, Display(display_description)));
    }

    //-------------------------------------------------------------------------
    DisplayManager::~DisplayManager()
    {
    }

    //-------------------------------------------------------------------------
    rex::not_null<const Display*> DisplayManager::set_active(int32 displayIndex, int32 modeIndex)
    {
        auto it = std::find_if(std::begin(m_displays), std::end(m_displays), [displayIndex](const auto& pair)
                               {
                                   return pair.second.get_display_index() == displayIndex;
                               });

        if (it != std::cend(m_displays))
        {
            it->second.set_active_mode(modeIndex);  // Activate display mode

            m_active_display = &(*it).second;
        }

        return m_active_display;
    }

    //-------------------------------------------------------------------------
    rex::not_null<const Display*> DisplayManager::get_active() const
    {
        return m_active_display;
    }

    //-------------------------------------------------------------------------
    int32 DisplayManager::display_count() const
    {
        return rtl::numeric_cast<int32>(m_displays.size());
    }

    //-------------------------------------------------------------------------
    int32 DisplayManager::display_mode_count() const
    {
        int32 total_display_mode_count = 0;

        for (auto& pair : m_displays)
        {
            total_display_mode_count += pair.second.get_display_mode_count();
        }

        return total_display_mode_count;
    }

    //-------------------------------------------------------------------------
    int32 DisplayManager::display_mode_count(int32 displayIndex) const
    {
        auto it = m_displays.find(displayIndex);
        if (it != std::cend(m_displays))
        {
            return it->second.get_display_mode_count();
        }

        // R_WARN("Display with index: {0}, was not found", displayIndex);
        return 0;
    }
}