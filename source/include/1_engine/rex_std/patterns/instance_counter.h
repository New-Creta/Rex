#pragma once

namespace rex
{
    template <typename T>
    class InstanceCounter
    {
    public:
        virtual ~InstanceCounter();

        size_t get_instance_id() const;
        static size_t get_total_instance_count();

    protected:
        InstanceCounter();

    private:
        size_t m_instance_id;
        static size_t s_total_instance_count;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    size_t rex::InstanceCounter<T>::s_total_instance_count = 0;

    //-------------------------------------------------------------------------
    template <typename T>
    rex::InstanceCounter<T>::InstanceCounter()
    {
        m_instance_id = s_total_instance_count;
        ++s_total_instance_count;
    }
    //-------------------------------------------------------------------------
    template <typename T>
    rex::InstanceCounter<T>::~InstanceCounter()
    {
        --s_total_instance_count;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    size_t rex::InstanceCounter<T>::get_instance_id() const
    {
        return m_instance_id;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    size_t rex::InstanceCounter<T>::get_total_instance_count()
    {
        return s_total_instance_count;
    }
}