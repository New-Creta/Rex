#pragma once


#include "core/format.h"

namespace rex
{
    namespace dxgi
    {
        class Format : public rex::Format
        {
        public:
            Format();
            Format(rex::FormatType type);
            ~Format() final;

            uint32 get() const final;

            static uint32 convertToDXGIFormatType(rex::FormatType type);
            static rex::FormatType convertToFormatType(uint32 type);
        };
    }
}