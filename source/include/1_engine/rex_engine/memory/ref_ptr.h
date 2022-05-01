#pragma once

#include "memory/ref_ptr_utilities.h"
#include "memory/ref_counted_object.h"

namespace rex
{
    template <typename T>
    class ref_ptr
    {
        template <class TOther>
        friend class ref_ptr;

    public:
        //-------------------------------------------------------------------------
        static bool deep_compare(const ref_ptr<T>& a, const ref_ptr<T>& b)
        {
            if (!a.m_instance || !b.m_instance)
            {
                return false;
            }

            return *a.m_instance == *b.m_instance;
        }

        //-------------------------------------------------------------------------
        ref_ptr()
            : m_instance(nullptr)
        {
            static_assert(std::is_base_of<IRefCountedObject, T>::value, "Class does not inherit from \"IRefCountedObject\"!");
        }
        //-------------------------------------------------------------------------
        ref_ptr(std::nullptr_t n)
            : m_instance(n)
        {
            static_assert(std::is_base_of<IRefCountedObject, T>::value, "Class does not inherit from \"IRefCountedObject\"!");
        }

        //-------------------------------------------------------------------------
        ref_ptr(T* instance)
            : m_instance(instance)
        {
            static_assert(std::is_base_of<IRefCountedObject, T>::value, "Class does not inherit from \"IRefCountedObject\"!");

            increment_reference();
        }
        //-------------------------------------------------------------------------
        template <typename TOther>
        explicit ref_ptr(TOther* instance)
            : m_instance(instance)
        {
            static_assert(std::is_base_of<IRefCountedObject, T>::value, "Class does not inherit from \"IRefCountedObject\"!");

            increment_reference();
        }

        //-------------------------------------------------------------------------
        ref_ptr(const ref_ptr& other)
        {
            static_assert(std::is_base_of<IRefCountedObject, T>::value, "Class does not inherit from \"IRefCountedObject\"!");

            m_instance = other.m_instance;
            increment_reference();
        }
        //-------------------------------------------------------------------------
        template <typename TOther>
        explicit ref_ptr(const ref_ptr<TOther>& other)
        {
            static_assert(std::is_base_of<IRefCountedObject, T>::value, "Class does not inherit from \"IRefCountedObject\"!");

            m_instance = static_cast<T*>(other.m_instance);
            increment_reference();
        }

        //-------------------------------------------------------------------------
        ref_ptr(ref_ptr&& other)
        {
            static_assert(std::is_base_of<IRefCountedObject, T>::value, "Class does not inherit from \"IRefCountedObject\"!");

            m_instance = other.m_instance;
            other.m_instance = nullptr;
        }
        //-------------------------------------------------------------------------
        template <typename TOther>
        explicit ref_ptr(ref_ptr<TOther>&& other)
        {
            static_assert(std::is_base_of<IRefCountedObject, T>::value, "Class does not inherit from \"IRefCountedObject\"!");

            m_instance = static_cast<T*>(other.m_instance);
            other.m_instance = nullptr;
        }

        //-------------------------------------------------------------------------
        ~ref_ptr()
        {
            decrement_reference();
        }

        //-------------------------------------------------------------------------
        ref_ptr& operator=(std::nullptr_t)
        {
            decrement_reference();
            m_instance = nullptr;
            return *this;
        }

        //-------------------------------------------------------------------------
        ref_ptr& operator=(const ref_ptr& other)
        {
            ref_ptr(other).swap(*this);
            return *this;
        }
        //-------------------------------------------------------------------------
        template <typename TOther>
        ref_ptr& operator=(const ref_ptr<TOther>& other)
        {
            ref_ptr(other).swap(*this);
            return *this;
        }

        //-------------------------------------------------------------------------
        ref_ptr& operator=(ref_ptr&& other)
        {
            ref_ptr(std::move(other)).swap(*this);
            return *this;
        }
        //-------------------------------------------------------------------------
        template <typename TOther>
        ref_ptr& operator=(ref_ptr<TOther>&& other)
        {
            ref_ptr(std::move(other)).swap(*this);
            return *this;
        }

        //-------------------------------------------------------------------------
        operator bool()
        {
            return m_instance != nullptr;
        }
        //-------------------------------------------------------------------------
        operator bool() const
        {
            return m_instance != nullptr;
        }

        //-------------------------------------------------------------------------
        T* operator->()
        {
            return m_instance;
        }
        //-------------------------------------------------------------------------
        const T* operator->() const
        {
            return m_instance;
        }

        //-------------------------------------------------------------------------
        T& operator*()
        {
            return *m_instance;
        }
        //-------------------------------------------------------------------------
        const T& operator*() const
        {
            return *m_instance;
        }

        //-------------------------------------------------------------------------
        bool operator==(const ref_ptr<T>& other) const
        {
            return m_instance == other.m_instance;
        }
        //-------------------------------------------------------------------------
        bool operator!=(const ref_ptr<T>& other) const
        {
            return !(*this == other);
        }

        //-------------------------------------------------------------------------
        T* get()
        {
            return m_instance;
        }
        //-------------------------------------------------------------------------
        const T* get() const
        {
            return m_instance;
        }

        //-------------------------------------------------------------------------
        void reset()
        {
            ref_ptr().swap(*this);
        }
        //-------------------------------------------------------------------------
        void reset(T* instance)
        {
            ref_ptr(instance).swap(*this);
        }

        //-------------------------------------------------------------------------
        void swap(ref_ptr<T>& other)
        {
            std::swap(m_instance, other.m_instance);
        }

    private:
        //-------------------------------------------------------------------------
        void increment_reference() const
        {
            if (m_instance)
            {
                m_instance->increment_reference();

                memory::add_to_reference_pool(m_instance);
            }
        }
        //-------------------------------------------------------------------------
        void decrement_reference() const
        {
            if (m_instance)
            {
                m_instance->decrement_reference();
                if (m_instance->get_reference_count() == 0)
                {
                    memory::remove_from_reference_pool(m_instance);

                    delete m_instance;
                    m_instance = nullptr;
                }
            }
        }

        mutable T* m_instance;
    };

#ifndef __EMSCRIPTEN__
    //-------------------------------------------------------------------------
    template <class T, class... Args, std::enable_if_t<!std::is_array_v<T>, int> = 0>
    ref_ptr<T> make_ref(Args&&... _Args)
    {
        return ref_ptr<T>(new T(std::forward<Args>(_Args)...));
    }
#else
    //-------------------------------------------------------------------------
    template <class T, class... Args>
    ref_ptr<T> make_ref(Args&&... _Args)
    {
        return ref_ptr<T>(new T(std::forward<Args>(_Args)...));
    }
#endif
}