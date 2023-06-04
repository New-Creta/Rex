#pragma once

#if REX_NO_LOGGING

  #define DEFINE_LOG_CATEGORY(CategoryName, ...) FNoLoggingCategory CategoryName;

//-------------------------------------------------------------------------
// Logging features

  #define REX_FATAL(...)       (0)
  #define REX_ERROR(...)       (0)
  #define REX_WARN(...)        (0)
  #define REX_LOG(...)         (0)
  #define REX_VERBOSE(...)     (0)
  #define REX_VERYVERBOSE(...) (0)

  #define REX_FATAL_X(...)       (0)
  #define REX_ERROR_X(...)       (0)
  #define REX_WARN_X(...)        (0)
  #define REX_LOG_X(...)         (0)
  #define REX_VERBOSE_X(...)     (0)
  #define REX_VERYVERBOSE_X(...) (0)

#else

  #include "rex_engine/diagnostics/logging/log_category.h"  // IWYU pragma: keep
  #include "rex_engine/diagnostics/logging/log_functions.h" // IWYU pragma: keep
  #include "rex_engine/diagnostics/logging/log_verbosity.h" // IWYU pragma: keep

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

  #define DEFINE_LOG_CATEGORY(Name, Verbosity)       inline const rex::LogCategory Name(#Name, Verbosity, rex::IsAsync::no)  // NOLINT(fuchsia-statically-constructed-objects)
  #define DEFINE_LOG_CATEGORY_ASYNC(Name, Verbosity) inline const rex::LogCategory Name(#Name, Verbosity, rex::IsAsync::yes) // NOLINT(fuchsia-statically-constructed-objects)
} // namespace rex

//-------------------------------------------------------------------------
// Logging features

  #define REX_FATAL(CategoryName, ...)       rex::trace_log(CategoryName, rex::LogVerbosity::Fatal, __VA_ARGS__)
  #define REX_ERROR(CategoryName, ...)       rex::trace_log(CategoryName, rex::LogVerbosity::Error, __VA_ARGS__)
  #define REX_WARN(CategoryName, ...)        rex::trace_log(CategoryName, rex::LogVerbosity::Warning, __VA_ARGS__)
  #define REX_LOG(CategoryName, ...)         rex::trace_log(CategoryName, rex::LogVerbosity::Log, __VA_ARGS__)
  #define REX_VERBOSE(CategoryName, ...)     rex::trace_log(CategoryName, rex::LogVerbosity::Verbose, __VA_ARGS__)
  #define REX_VERYVERBOSE(CategoryName, ...) rex::trace_log(CategoryName, rex::LogVerbosity::VeryVerbose, __VA_ARGS__)

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
  #define REX_LOG_X(CategoryName, cond, ...)                                                                                                                                                                                                             \
    [&]()                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                    \
      if(!(cond))                                                                                                                                                                                                                                        \
      {                                                                                                                                                                                                                                                  \
        REX_LOG(CategoryName, __VA_ARGS__);                                                                                                                                                                                                              \
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

#endif