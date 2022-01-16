#pragma once

#include <chrono>

namespace rex 
{
    using fseconds      = std::chrono::duration<float>;
    using fmiliseconds  = std::chrono::duration<float, std::milli>;

    class DeltaTime
    {
    public:
        //-------------------------------------------------------------------------
        DeltaTime()
            : m_last_time_point(std::chrono::steady_clock::now())
            , m_current_time_point(std::chrono::steady_clock::now() - m_last_time_point)
        {}

        //-------------------------------------------------------------------------
        void update()
        {
            std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();

            m_current_time_point = fmiliseconds(current - m_last_time_point);
            m_last_time_point    = current;
        }
                              
        //--------------------------------------------------------------------------------------------
        float toSeconds() const          
        {
            return fseconds { m_current_time_point }.count(); 
        }
        //--------------------------------------------------------------------------------------------
        float toMilliseconds() const     
        { 
            return m_current_time_point.count(); 
        }

    private:
        std::chrono::steady_clock::time_point m_last_time_point;
        fmiliseconds m_current_time_point;
    };
}