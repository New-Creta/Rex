#include "dxgi/core/dxgioutput.h"
#include "dxgi/core/dxgiformat.h"

#include "string/stringconversions.h"

#include "dxgi/core/dxgiadapter.h"

namespace
{
    const uint32 DEVICE_NAME_SIZE = 32;
    
    //-------------------------------------------------------------------------
    std::map<rex::FormatType, uint32> loadDisplayModeCount1(const rex::dx::ComPtr<IDXGIOutput6>& output)
    {
        UINT count = 0;
        UINT flags = 0;

        std::map<rex::FormatType, uint32> supported_formats;
        for (uint32 i = 0; i < static_cast<uint32>(rex::FormatType::MAX_FORMAT); ++i)
        {
            rex::FormatType format_type = static_cast<rex::FormatType>(i);
            DXGI_FORMAT format = (DXGI_FORMAT)rex::dxgi::Format::convertToDXGIFormatType(format_type);

            output->GetDisplayModeList1(format, flags, &count, nullptr);
            if (count > 0)
                supported_formats[format_type] = count;
        }

        return supported_formats;
    }
    //-------------------------------------------------------------------------
    std::vector<rex::DisplayMode> loadDisplayMode1(const rex::dx::ComPtr<IDXGIOutput6>& output, rex::dxgi::Format format, uint32 count)
    {
        RX_ASSERT_X(count > 0, "There needs to be at least 1 display mode");

        UINT flags = 0;

        std::vector<DXGI_MODE_DESC1> modes(count);
        output->GetDisplayModeList1(format.as<DXGI_FORMAT>(), flags, &count, modes.data());

        std::vector<rex::DisplayMode> display_modes(count);
        for (uint32 i = 0; i < count; ++i)
        {
            display_modes[i].width = modes[i].Width;
            display_modes[i].height = modes[i].Height;
            display_modes[i].format = rex::dxgi::Format::convertToFormatType(static_cast<uint32>(modes[i].Format));
            display_modes[i].refresh_rate = rex::Rational(modes[i].RefreshRate.Numerator, modes[i].RefreshRate.Denominator);
        }

        return display_modes;
    }

    //-------------------------------------------------------------------------
    std::map<rex::FormatType, uint32> loadDisplayModeCount(const rex::dx::ComPtr<IDXGIOutput>& output)
    {
        UINT count = 0;
        UINT flags = DXGI_ENUM_MODES_INTERLACED;

        std::map<rex::FormatType, uint32> supported_formats;
        for (uint32 i = 0; i < static_cast<uint32>(rex::FormatType::MAX_FORMAT); ++i)
        {
            rex::FormatType format_type = static_cast<rex::FormatType>(i);
            DXGI_FORMAT format = (DXGI_FORMAT)rex::dxgi::Format::convertToDXGIFormatType(format_type);

            output->GetDisplayModeList(format, flags, &count, nullptr);
            if (count > 0)
                supported_formats[format_type]++;
        }

        return supported_formats;
    }
    //-------------------------------------------------------------------------
    std::vector<rex::DisplayMode> loadDisplayMode(const rex::dx::ComPtr<IDXGIOutput>& output, rex::dxgi::Format format, uint32 count)
    {
        RX_ASSERT_X(count > 0, "There needs to have at least 1 display mode");

        UINT flags = 0;

        std::vector<DXGI_MODE_DESC> modes(count);
        output->GetDisplayModeList(format.as<DXGI_FORMAT>(), flags, &count, &modes[0]);

        std::vector<rex::DisplayMode> display_modes(count);
        for (uint32 i = 0; i < count; ++i)
        {
            display_modes[i].width = modes[i].Width;
            display_modes[i].height = modes[i].Height;
            display_modes[i].format = rex::dxgi::Format::convertToFormatType(static_cast<uint32>(modes[i].Format));
            display_modes[i].refresh_rate = rex::Rational(modes[i].RefreshRate.Numerator, modes[i].RefreshRate.Denominator);
        }

        return display_modes;
    }

    //-------------------------------------------------------------------------
    uint32 calculateVersion(const rex::dx::ComPtr<IDXGIOutput> baseOutput)
    {
        if (rex::dx::convertTo<IDXGIOutput6>(baseOutput))   return 6;
        if (rex::dx::convertTo<IDXGIOutput5>(baseOutput))   return 5;
        if (rex::dx::convertTo<IDXGIOutput4>(baseOutput))   return 4;
        if (rex::dx::convertTo<IDXGIOutput3>(baseOutput))   return 3;
        if (rex::dx::convertTo<IDXGIOutput2>(baseOutput))   return 2;
        if (rex::dx::convertTo<IDXGIOutput1>(baseOutput))   return 1;

        return 0;
    }
}

//-------------------------------------------------------------------------
rex::dxgi::Output::Output(rex::dx::ComPtr<IDXGIOutput>&& output, const dxgi::Adapter* adapter)
    : ComObject(std::move(output), calculateVersion(output))
    , m_adapter(adapter)
{
    switch (getVersion())
    {
    case 6: loadOutput6();   break;
    case 5:
    case 4:
    case 3:
    case 2:
    case 1:
    case 0: loadOutput0();   break;
    }
}
//-------------------------------------------------------------------------
rex::dxgi::Output::~Output() = default;

//-------------------------------------------------------------------------
const std::string& rex::dxgi::Output::getName() const
{
    return m_name;
}
//-------------------------------------------------------------------------
const rex::Gpu* rex::dxgi::Output::getAdapter() const
{
    return m_adapter;
}

//-------------------------------------------------------------------------
const std::vector<rex::DisplayMode> rex::dxgi::Output::getDisplayModes(rex::FormatType type) const
{
    if (m_display_modes.find(type) == std::cend(m_display_modes))
        return {};

    return m_display_modes.at(type);
}
//-------------------------------------------------------------------------
const std::map<rex::FormatType, std::vector<rex::DisplayMode>>& rex::dxgi::Output::getDisplayModes() const
{
    return m_display_modes;
}

//-------------------------------------------------------------------------
void rex::dxgi::Output::loadOutput6()
{
    rex::dx::ComPtr<IDXGIOutput6> output_6 = getAs<IDXGIOutput6>();

    DXGI_OUTPUT_DESC1 output_desc;
    output_6->GetDesc1(&output_desc);
    m_name = rex::conversions::toMultiByte(output_desc.DeviceName, DEVICE_NAME_SIZE);

    auto supported_formats = loadDisplayModeCount1(output_6);
    for (const auto& [format_type, format_count] : supported_formats)
    {
        m_display_modes.insert(std::make_pair(format_type, loadDisplayMode(getComPtr(), rex::dxgi::Format(format_type), format_count)));
    }

    if (m_display_modes.empty())
    {
        RX_WARN("No display modes loaded for output {}", m_name);
    }
}
//-------------------------------------------------------------------------
void rex::dxgi::Output::loadOutput0()
{
    DXGI_OUTPUT_DESC output_desc;
    get()->GetDesc(&output_desc);
    m_name = rex::conversions::toMultiByte(output_desc.DeviceName, DEVICE_NAME_SIZE);

    auto supported_formats = loadDisplayModeCount(get());
    for (const auto& [format_type, format_count] : supported_formats)
    {
        m_display_modes.insert(std::make_pair(format_type, loadDisplayMode(getComPtr(), rex::dxgi::Format(format_type), format_count)));
    }

    if (m_display_modes.empty())
    {
        RX_WARN("No display modes loaded for output {}", m_name);
    }
}
