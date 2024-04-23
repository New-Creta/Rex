#pragma once

#include "rex_engine/diagnostics/assert.h"

#include "rex_std/memory.h"

namespace rex
{
  template <typename T>
  class StaticSingleton
  {
  public:
    StaticSingleton(const StaticSingleton&)            = delete;
    StaticSingleton& operator=(const StaticSingleton&) = delete;
    StaticSingleton(StaticSingleton&&)                 = delete;
    StaticSingleton& operator=(StaticSingleton&&)      = delete;

    virtual ~StaticSingleton() = default;

    static bool has_instance();
    static T* instance();

    template <class... Args>
    static void create_instance(Args&... args);
    static void destroy_instance();

  private:
    static rsl::unique_ptr<T> m_instance;
  };

  //-------------------------------------------------------------------------
  template <typename T>
  rsl::unique_ptr<T> rex::StaticSingleton<T>::m_instance = nullptr;

  //-------------------------------------------------------------------------
  template <typename T>
  bool rex::StaticSingleton<T>::has_instance()
  {
    return m_instance != nullptr;
  }
  //-------------------------------------------------------------------------
  template <typename T>
  T* rex::StaticSingleton<T>::instance()
  {
    REX_ASSERT_X(m_instance != nullptr, "Singleton instance was not created.");

    return m_instance.get();
  }

  //-------------------------------------------------------------------------
  template <typename T>
  template <class... Args>
  void rex::StaticSingleton<T>::create_instance(Args&... args)
  {
    REX_ASSERT_X(m_instance == nullptr, "Singleton instance already created.");

    m_instance = rsl::make_unique<T>(rsl::forward<Args>(args)...);
  }
  //-------------------------------------------------------------------------
  template <typename T>
  void rex::StaticSingleton<T>::destroy_instance()
  {
    m_instance.reset(nullptr);
  }

  template <typename T>
  class DynamicSingleton
  {
  public:
    DynamicSingleton(const DynamicSingleton&)            = delete;
    DynamicSingleton& operator=(const DynamicSingleton&) = delete;
    DynamicSingleton(DynamicSingleton&&)                 = delete;
    DynamicSingleton& operator=(DynamicSingleton&&)      = delete;

    virtual ~DynamicSingleton() = default;

    static bool has_instance();
    static T* instance();

  private:
    static rsl::unique_ptr<T> m_instance;
  };

  //-------------------------------------------------------------------------
  template <typename T>
  rsl::unique_ptr<T> rex::DynamicSingleton<T>::m_instance = nullptr;

  //-------------------------------------------------------------------------
  template <typename T>
  bool rex::DynamicSingleton<T>::has_instance()
  {
    return m_instance != nullptr;
  }
  //-------------------------------------------------------------------------
  template <typename T>
  T* rex::DynamicSingleton<T>::instance()
  {
    if(!has_instance())
    {
      m_instance = rsl::make_unique<T>();
    }

    return m_instance.get();
  }
} // namespace rex