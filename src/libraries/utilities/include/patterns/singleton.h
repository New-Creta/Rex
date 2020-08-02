#pragma once

#include <memory>
#include <assert.h>

namespace rex
{
    template<typename T>
    class Singleton
    {
    public:
        Singleton(Singleton const&) = delete;
        Singleton& operator = (Singleton const&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator = (Singleton&&) = delete;

        virtual ~Singleton() = default;

        static bool hasInstance();
        static T* instance();

        template <class ... Args>
        static void createInstance(Args&... args);
        static void destroyInstance();
    private:
        static std::unique_ptr<T> m_instance;
    };

    //-------------------------------------------------------------------------
    template<typename T> std::unique_ptr<T> rex::Singleton<T>::m_instance = nullptr;

    //-------------------------------------------------------------------------
    template<typename T>
    bool rex::Singleton<T>::hasInstance()
    {
        return m_instance != nullptr;
    }
    //-------------------------------------------------------------------------
    template<typename T>
    T* rex::Singleton<T>::instance()
    {
        assert(m_instance != nullptr && "Singleton instance was not created.");

        return m_instance.get();
    }

    //-------------------------------------------------------------------------
    template<typename T>
    template<class ... Args>
    void rex::Singleton<T>::createInstance(Args&... args)
    {
        assert(m_instance == nullptr && "Singleton instance already created.");

        m_instance = std::make_unique<T>(std::forward<Args>(args)...);
    }
    //-------------------------------------------------------------------------
    template<typename T>
    void rex::Singleton<T>::destroyInstance()
    {
        m_instance.reset(nullptr);
    }
}