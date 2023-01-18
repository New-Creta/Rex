#pragma once

#include "rex_engine/timing/tickcount.h"

namespace rex
{
    class Interval
    {
    public:
        //-------------------------------------------------------------------------
        Interval() 
            : m_initial(getTickCount())
        {}
        //-------------------------------------------------------------------------
        ~Interval() = default;

        //-------------------------------------------------------------------------
        inline int64_t value() const
        {
            return getTickCount() - m_initial;
        }

    private:
        int64_t m_initial;
    };

}