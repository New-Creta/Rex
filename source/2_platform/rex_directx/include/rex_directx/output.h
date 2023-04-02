#pragma once

#include "dxgi/core/dxgicomobject.h"

#include "core/output.h"
#include "core/displaymode.h"

#include "dxgi/core/dxgiutil.h"

namespace rex
{
    enum class FormatType;

    namespace dxgi
    {
		class Adapter;

        class Output : public rex::Output, public rex::dxgi::ComObject<IDXGIOutput>
        {
        public:
            Output(rex::dx::ComPtr<IDXGIOutput>&& output, const dxgi::Adapter* adapter);
            ~Output() final;

            const std::string& getName() const final;
            const rex::Gpu* getAdapter() const final;

            const std::vector<DisplayMode> getDisplayModes(FormatType type) const final;
            const std::map<FormatType, std::vector<DisplayMode>>& getDisplayModes() const final;
 
        private:
            void loadOutput6();
            void loadOutput0();

            std::string m_name;
            const dxgi::Adapter* m_adapter;
            std::map<FormatType, std::vector<DisplayMode>> m_display_modes;

        };
    }
}