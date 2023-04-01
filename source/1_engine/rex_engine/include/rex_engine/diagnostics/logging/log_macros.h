#pragma once

#if REX_NO_LOGGING

  #define REX_LOG_ACTIVE(...)        (0)
  #define REX_GET_LOG_VERBOSITY(...) (LogVerbosity::NoLogging)
  #define REX_SET_LOG_VERBOSITY(...)

  #define DECLARE_LOG_CATEGORY_EXTERN(CategoryName, DefaultVerbosity, CompileTimeVerbosity) extern FNoLoggingCategory CategoryName;
  #define DEFINE_LOG_CATEGORY(CategoryName, ...)                                            FNoLoggingCategory CategoryName;

//-------------------------------------------------------------------------
// Logging features

  #define REX_FATAL(...)          (0)
  #define REX_ERROR(...)          (0)
  #define REX_WARN(...)           (0)
  #define REX_LOG(...)            (0)
  #define REX_VERBOSE(...)        (0)
  #define REX_VERYVERBOSE(...)    (0)

  #define REX_FATAL_X(...)        (0)
  #define REX_ERROR_X(...)        (0)
  #define REX_WARN_X(...)         (0)
  #define REX_LOG_X(...)          (0)
  #define REX_VERBOSE_X(...)      (0)
  #define REX_VERYVERBOSE_X(...)  (0)

#else

  #include "rex_engine/diagnostics/logging/log_category.h"
  #include "rex_engine/diagnostics/logging/log_verbosity.h"
  #include "rex_engine/diagnostics/logging/logger.h"

namespace rex
{
  namespace internal
  {
    template <int VerbosityToCheck, typename CategoryType>
    inline bool isLogActive(const CategoryType& Category)
    {
      return !Category.IsSuppressed((LogVerbosity)VerbosityToCheck);
    }
  } // namespace internal

  /**
   * A predicate that returns true if the given logging category is active (logging) at a given verbosity level
   * @param CategoryName name of the logging category
   * @param Verbosity, verbosity level to test against
   */
  #define REX_LOG_ACTIVE(CategoryName, Verbosity)        (rex::internal::isLogActive<(int32)rex::##Verbosity>(CategoryName))

  /**
   * Retrieve the verbosity level of a specific category
   */
  #define REX_GET_LOG_VERBOSITY(CategoryName)            CategoryName.GetVerbosity()

  /**
   * Set the verbosity level of a specific category
   */
  #define REX_SET_LOG_VERBOSITY(CategoryName, Verbosity) CategoryName.SetVerbosity(rex::##Verbosity);

  /**
   * A macro to declare a logging category as a C++ "extern", usually declared in the header and paired with DEFINE_LOG_CATEGORY in the source. Accessible by all files that include the header.
   * @param CategoryName, category to declare
   * @param DefaultVerbosity, default run time verbosity
   */
  #define DECLARE_LOG_CATEGORY_EXTERN(CategoryName, DefaultVerbosity)                                                                                                                                                                                    \
    extern struct LogCategory##CategoryName : public rex::LogCategory<(int)rex::DefaultVerbosity>                                                                                                                                                        \
    {                                                                                                                                                                                                                                                    \
      inline LogCategory##CategoryName()                                                                                                                                                                                                                 \
          : LogCategory<(int)rex::DefaultVerbosity>(rsl::string(#CategoryName))                                                                                                                                                                          \
      {                                                                                                                                                                                                                                                  \
      }                                                                                                                                                                                                                                                  \
    } CategoryName;

  /**
   * A macro to define a logging category, usually paired with DECLARE_LOG_CATEGORY_EXTERN from the header.
   * @param CategoryName, category to define
   */
  #define DEFINE_LOG_CATEGORY(CategoryName) LogCategory##CategoryName CategoryName;
} // namespace rex

//-------------------------------------------------------------------------
// Logging features

  #define REX_FATAL(CategoryName, ...)       rex::trace_log(CategoryName, rex::LogVerbosity::Fatal, __VA_ARGS__)
  #define REX_ERROR(CategoryName, ...)       rex::trace_log(CategoryName, rex::LogVerbosity::Error, __VA_ARGS__)
  #define REX_WARN(CategoryName, ...)        rex::trace_log(CategoryName, rex::LogVerbosity::Warning, __VA_ARGS__)
  #define REX_LOG(CategoryName, ...)         rex::trace_log(CategoryName, rex::LogVerbosity::Log, __VA_ARGS__)
  #define REX_VERBOSE(CategoryName, ...)     rex::trace_log(CategoryName, rex::LogVerbosity::Verbose, __VA_ARGS__)
  #define REX_VERYVERBOSE(CategoryName, ...) rex::trace_log(CategoryName, rex::LogVerbosity::VeryVerbose, __VA_ARGS__)

  #define REX_FATAL_X(CategoryName, cond, ...)                                                                                      \
    [&]()                                                                                                                           \
    {                                                                                                                               \
        if (!(cond))                                                                                                                \
        {                                                                                                                           \
            REX_FATAL(CategoryName, __VA_ARGS__);                                                                                            \
            return true;                                                                                                            \
        }                                                                                                                           \
        return false;                                                                                                               \
    }

  #define REX_ERROR_X(CategoryName, cond, ...)                                                                                      \
    [&]()                                                                                                                           \
    {                                                                                                                               \
        if (!(cond))                                                                                                                \
        {                                                                                                                           \
            REX_ERROR(CategoryName, __VA_ARGS__);                                                                                            \
            return true;                                                                                                            \
        }                                                                                                                           \
        return false;                                                                                                               \
    }
  #define REX_WARN_X(CategoryName, cond, ...)                                                                                       \
    [&]()                                                                                                                           \
    {                                                                                                                               \
        if (!(cond))                                                                                                                \
        {                                                                                                                           \
            REX_WARN(CategoryName, __VA_ARGS__);                                                                                             \
            return true;                                                                                                            \
        }                                                                                                                           \
        return false;                                                                                                               \
    }
  #define REX_LOG_X(CategoryName, cond, ...)                                                                                        \
    [&]()                                                                                                                           \
    {                                                                                                                               \
        if (!(cond))                                                                                                                \
        {                                                                                                                           \
            REX_LOG(CategoryName, __VA_ARGS__);                                                                                              \
            return true;                                                                                                            \
        }                                                                                                                           \
        return false;                                                                                                               \
    }
  #define REX_VERBOSE_X(CategoryName, cond, ...)                                                                                    \
    [&]()                                                                                                                           \
    {                                                                                                                               \
        if (!(cond))                                                                                                                \
        {                                                                                                                           \
            REX_VERBOSE(CategoryName, __VA_ARGS__);                                                                                          \
            return true;                                                                                                            \
        }                                                                                                                           \
        return false;                                                                                                               \
    }
  #define REX_VERYVERBOSE_X(CategoryName, cond, ...)                                                                                \
    [&]()                                                                                                                           \
    {                                                                                                                               \
        if (!(cond))                                                                                                                \
        {                                                                                                                           \
            REX_VERYVERBOSE(CategoryName, __VA_ARGS__);                                                                                      \
            return true;                                                                                                            \
        }                                                                                                                           \
        return false;                                                                                                               \
    }

#endif