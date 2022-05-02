#pragma once

namespace rex
{
    namespace internal_ptr_deleter
    {
        // Ensure that an internal_scope_ptr based object will automatically self destroy upon going out of scope.
        template <class T>
        void deleter(T* victim)
        {
            delete victim;
        }
    }

    // Alias to a std::unique_ptr with a custom deleter signature.
    template <class T, class Deleter = void (*)(T*)>
    using internal_scope_ptr = std::unique_ptr<T, Deleter>;

    // Alias to a std::unique_ptr with a custom deleter signature.
    template <class T, class Deleter = void (*)(T*)>
    using internal_ref_ptr = std::shared_ptr<T>;

    // Create a new instance of an internal_scope_ptr type.
    // The factory method will forward all its arguments on to the target object constructor.
    template <class T, class... Args>
    inline internal_scope_ptr<T> make_internal_scope_ptr(Args&&... args)
    {
        return internal_scope_ptr<T>(new T(std::forward<Args>(args)...), &internal_ptr_deleter::deleter<T>);
    }

    // Create a new instance of an internal_ref_ptr type.
    // The factory method will forward all its arguments on to the target object constructor.
    template <class T, class... Args>
    inline internal_ref_ptr<T> make_internal_ref_ptr(Args&&... args)
    {
        return internal_ref_ptr<T>(new T(std::forward<Args>(args)...), &internal_ptr_deleter::deleter<T>);
    }
}