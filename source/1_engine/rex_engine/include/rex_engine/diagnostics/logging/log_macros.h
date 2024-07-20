#pragma once

#include "rex_engine/diagnostics/logging/log_category.h"                                             // IWYU pragma: keep
#include "rex_engine/diagnostics/logging/log_functions.h"                                            // IWYU pragma: keep
#include "rex_engine/diagnostics/logging/log_verbosity.h"                                            // IWYU pragma: keep

namespace rex
{
  namespace internal
  {
    template <int VerbosityToCheck, typename CategoryType>
    inline bool is_log_active(const CategoryType& category)
    {
      return !category.is_suppressed((LogVerbosity)VerbosityToCheck);
    }
  } // namespace internal

#define DEFINE_LOG_CATEGORY(Name)       inline const rex::LogCategory Name(#Name, rex::IsAsync::no)  // NOLINT(fuchsia-statically-constructed-objects)
#define DEFINE_LOG_CATEGORY_ASYNC(Name) inline const rex::LogCategory Name(#Name, rex::IsAsync::yes) // NOLINT(fuchsia-statically-constructed-objects)
} // namespace rex

#if REX_NO_LOGGING

//-------------------------------------------------------------------------
// Logging features

  #define REX_FATAL(...)       (0)
  #define REX_ERROR(...)       (0)
  #define REX_WARN(...)        (0)
  #define REX_INFO(...)        (0)
  #define REX_VERBOSE(...)     (0)
  #define REX_VERYVERBOSE(...) (0)

  #define REX_FATAL_X(...)       (0)
  #define REX_ERROR_X(...)       (0)
  #define REX_WARN_X(...)        (0)
  #define REX_INFO_X(...)        (0)
  #define REX_VERBOSE_X(...)     (0)
  #define REX_VERYVERBOSE_X(...) (0)

  #define REX_FATAL_ONCE(CategoryName, ...)    
  #define REX_ERROR_ONCE(CategoryName, ...)   
  #define REX_WARN_ONCE(CategoryName, ...)    
  #define REX_INFO_ONCE(CategoryName, ...)    
  #define REX_VERBOSE_ONCE(CategoryName, ...) 
  #define REX_VERYVERBOSE_ONCE(CategoryName, ...)
  
  // Only log once during the duration of the program, on the first time the condition is true
  #define REX_FATAL_ONCE_X(CategoryName, cond, ...)       false
  #define REX_ERROR_ONCE_X(CategoryName, cond, ...)       false
  #define REX_WARN_ONCE_X(CategoryName, cond, ...)        false
  #define REX_INFO_ONCE_X(CategoryName, cond, ...)        false
  #define REX_VERBOSE_ONCE_X(CategoryName, cond, ...)     false
  #define REX_VERYVERBOSE_ONCE_X(CategoryName, cond, ...) false

#else

  //-------------------------------------------------------------------------
// Logging features

  #define REX_FATAL(CategoryName, ...)       rex::log::trace_log(CategoryName, rex::log::LogVerbosity::Critical, __VA_ARGS__)
  #define REX_ERROR(CategoryName, ...)       rex::log::trace_log(CategoryName, rex::log::LogVerbosity::Err, __VA_ARGS__)
  #define REX_WARN(CategoryName, ...)        rex::log::trace_log(CategoryName, rex::log::LogVerbosity::Warn, __VA_ARGS__)
  #define REX_INFO(CategoryName, ...)        rex::log::trace_log(CategoryName, rex::log::LogVerbosity::Info, __VA_ARGS__)
  #define REX_VERBOSE(CategoryName, ...)     rex::log::trace_log(CategoryName, rex::log::LogVerbosity::Debug, __VA_ARGS__)
  #define REX_VERYVERBOSE(CategoryName, ...) rex::log::trace_log(CategoryName, rex::log::LogVerbosity::Trace, __VA_ARGS__)

  #define REX_FATAL_X(CategoryName, cond, ...)                                                                                                                                                                                                           \
    [&]()                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                    \
      if(!(cond))                                                                                                                                                                                                                                        \
      {                                                                                                                                                                                                                                                  \
        REX_FATAL(CategoryName, __VA_ARGS__);                                                                                                                                                                                                            \
        return true;                                                                                                                                                                                                                                     \
      }                                                                                                                                                                                                                                                  \
      return false;                                                                                                                                                                                                                                      \
    }()

  #define REX_ERROR_X(CategoryName, cond, ...)                                                                                                                                                                                                           \
    [&]()                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                    \
      if(!(cond))                                                                                                                                                                                                                                        \
      {                                                                                                                                                                                                                                                  \
        REX_ERROR(CategoryName, __VA_ARGS__);                                                                                                                                                                                                            \
        return true;                                                                                                                                                                                                                                     \
      }                                                                                                                                                                                                                                                  \
      return false;                                                                                                                                                                                                                                      \
    }()
  #define REX_WARN_X(CategoryName, cond, ...)                                                                                                                                                                                                            \
    [&]()                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                    \
      if(!(cond))                                                                                                                                                                                                                                        \
      {                                                                                                                                                                                                                                                  \
        REX_WARN(CategoryName, __VA_ARGS__);                                                                                                                                                                                                             \
        return true;                                                                                                                                                                                                                                     \
      }                                                                                                                                                                                                                                                  \
      return false;                                                                                                                                                                                                                                      \
    }()
  #define REX_INFO_X(CategoryName, cond, ...)                                                                                                                                                                                                            \
    [&]()                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                    \
      if(!(cond))                                                                                                                                                                                                                                        \
      {                                                                                                                                                                                                                                                  \
        REX_INFO(CategoryName, __VA_ARGS__);                                                                                                                                                                                                             \
        return true;                                                                                                                                                                                                                                     \
      }                                                                                                                                                                                                                                                  \
      return false;                                                                                                                                                                                                                                      \
    }()
  #define REX_VERBOSE_X(CategoryName, cond, ...)                                                                                                                                                                                                         \
    [&]()                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                    \
      if(!(cond))                                                                                                                                                                                                                                        \
      {                                                                                                                                                                                                                                                  \
        REX_VERBOSE(CategoryName, __VA_ARGS__);                                                                                                                                                                                                          \
        return true;                                                                                                                                                                                                                                     \
      }                                                                                                                                                                                                                                                  \
      return false;                                                                                                                                                                                                                                      \
    }()
  #define REX_VERYVERBOSE_X(CategoryName, cond, ...)                                                                                                                                                                                                     \
    [&]()                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                    \
      if(!(cond))                                                                                                                                                                                                                                        \
      {                                                                                                                                                                                                                                                  \
        REX_VERYVERBOSE(CategoryName, __VA_ARGS__);                                                                                                                                                                                                      \
        return true;                                                                                                                                                                                                                                     \
      }                                                                                                                                                                                                                                                  \
      return false;                                                                                                                                                                                                                                      \
    }()

#include "rex_engine/engine/defines.h"

// Only log once during the doration of the program
#define REX_FATAL_ONCE(CategoryName, ...)       REX_EXECUTE_ONCE(REX_FATAL(CategoryName, __VA_ARGS__))
#define REX_ERROR_ONCE(CategoryName, ...)       REX_EXECUTE_ONCE(REX_ERROR(CategoryName, __VA_ARGS__))
#define REX_WARN_ONCE(CategoryName, ...)        REX_EXECUTE_ONCE(REX_WARN(CategoryName, __VA_ARGS__))
#define REX_INFO_ONCE(CategoryName, ...)        REX_EXECUTE_ONCE(REX_INFO(CategoryName, __VA_ARGS__))
#define REX_VERBOSE_ONCE(CategoryName, ...)     REX_EXECUTE_ONCE(REX_VERBOSE(CategoryName, __VA_ARGS__))
#define REX_VERYVERBOSE_ONCE(CategoryName, ...) REX_EXECUTE_ONCE(REX_VERYVERBOSE(CategoryName, __VA_ARGS__))

// Only log once during the duration of the program, on the first time the condition is true
#define REX_FATAL_ONCE_X(CategoryName, cond, ...)       if (cond) { REX_EXECUTE_ONCE(REX_FATAL(CategoryName, __VA_ARGS__)); }
#define REX_ERROR_ONCE_X(CategoryName, cond, ...)       if (cond) { REX_EXECUTE_ONCE(REX_ERROR(CategoryName, __VA_ARGS__)); }
#define REX_WARN_ONCE_X(CategoryName, cond, ...)        if (cond) { REX_EXECUTE_ONCE(REX_WARN(CategoryName, __VA_ARGS__)); }
#define REX_INFO_ONCE_X(CategoryName, cond, ...)        if (cond) { REX_EXECUTE_ONCE(REX_INFO(CategoryName, __VA_ARGS__)); }
#define REX_VERBOSE_ONCE_X(CategoryName, cond, ...)     if (cond) { REX_EXECUTE_ONCE(REX_VERBOSE(CategoryName, __VA_ARGS__)); }
#define REX_VERYVERBOSE_ONCE_X(CategoryName, cond, ...) if (cond) { REX_EXECUTE_ONCE(REX_VERYVERBOSE(CategoryName, __VA_ARGS__)); }

#endif