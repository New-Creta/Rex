#pragma once

#include "rex_directx/wrl/wrl_types.h"
#include "rex_std/type_traits.h"

#include <wrl.h>

namespace rex
{
    namespace wrl
    {
        //-------------------------------------------------------------------------
        template <typename To, typename From>
        wrl::com_ptr<To> convertTo(const wrl::com_ptr<From>& ptr)
        {
            wrl::com_ptr<To> pointer;
            ptr.As<To>(&pointer);
            return pointer;
        }

        template <typename T>
        class ComObject
        {
        public:
            ComObject();
            ComObject(wrl::com_ptr<T>&& object);

            virtual ~ComObject();

            /**
            * Retrieve the underlying com ptr
            */
            T*                          com_ptr();
            /**
            * Retrieve the underlying com ptr
            */
            const T*                    com_ptr() const;

            /**
                In order to pass along " const " objects to DirectX we are able to remove the " const " with this function.
                Since DirectX has a C API it does not know what " const " is.
            */
            T*                          c_ptr() const;

            /**
            * Reset the stored com shared ptr, this will decrement the recount on the stored com object
            * If we reach 0 the comobject will be destroyed
            */
            void                        reset();

            /**
            * Retrieve this com object as a different type
            */
            template <typename To>
            wrl::com_ptr<To>          as() const;

        private:
            wrl::com_ptr<T> m_pointer;
        };

        //-------------------------------------------------------------------------
        template <typename T>
        ComObject<T>::ComObject()
            : m_pointer()
        {}
        //-------------------------------------------------------------------------
        template <typename T>
        ComObject<T>::ComObject(rex::wrl::com_ptr<T>&& object)
            : m_pointer(rsl::move(object))
        {}

        //-------------------------------------------------------------------------
        template <typename T>
        ComObject<T>::~ComObject()
        {}

        //-------------------------------------------------------------------------
        template <typename T>
        T* ComObject<T>::com_ptr()
        {
            return m_pointer.Get();
        }
        //-------------------------------------------------------------------------
        template <typename T>
        const T* ComObject<T>::com_ptr() const
        {
            return m_pointer.Get();
        }

        //-------------------------------------------------------------------------
        template <typename T>
        T* ComObject<T>::c_ptr() const
        {
            static_assert(rsl::is_base_of_v<IUnknown, T>, "T is not a com object!");
            return const_cast<T*>(m_pointer.Get());
        }

        //-------------------------------------------------------------------------
        template <typename T>
        void ComObject<T>::reset()
        {
            m_pointer.Reset();
        }

        //-------------------------------------------------------------------------
        template <typename T>
        template <typename To>
        wrl::com_ptr<To> ComObject<T>::as() const
        {
            // T needs to be non-const
            // so we need to case away the const
            return convertTo<To, T>(c_ptr());
        }
    }
}