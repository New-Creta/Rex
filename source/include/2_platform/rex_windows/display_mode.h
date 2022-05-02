#pragma once

namespace rex
{
    struct DisplayModeDescription
    {
        DisplayModeDescription()
            : mode_index(-1)
            , width(-1)
            , height(-1)
            , refresh_rate(-1)
            , pixel_format(0)
            , bits_per_pixel(0)
        {
        }

        int32 mode_index;

        int32 width;
        int32 height;
        int32 refresh_rate;

        uint32 pixel_format;
        uint32 bits_per_pixel;
    };

    class DisplayMode
    {
    public:
        DisplayMode();
        DisplayMode(const DisplayMode& other);
        DisplayMode(DisplayMode&& other) noexcept;
        DisplayMode(const DisplayModeDescription& description);
        ~DisplayMode();

        DisplayMode& operator=(const DisplayMode& other);
        DisplayMode& operator=(DisplayMode&& other) noexcept;

        int32 get_mode_index() const;

        int32 get_width() const;
        int32 get_height() const;
        int32 get_refresh_rate() const;

        uint32 get_pixel_format() const;
        uint32 get_bits_per_pixel() const;

    private:
        DisplayModeDescription m_description;
    };
}