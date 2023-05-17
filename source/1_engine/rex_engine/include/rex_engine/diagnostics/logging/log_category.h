#pragma once

#include "rex_std/string.h" // for string_view

namespace rex
{
  enum class LogVerbosity;

  using LogCategoryName = rsl::string_view;

  /** Base class for all log categories. **/
  struct LogCategory
  {
    /**
     * Constructor, registers with the log suppression system and sets up the default values.
     * @param CategoryName, name of the category
     * @param InDefaultVerbosity, default verbosity for the category, may ignored and overridden by many other mechanisms
     **/
    LogCategory(const LogCategoryName& inCategoryName, LogVerbosity inDefaultVerbosity);

    /** Gets the category name **/
    const LogCategoryName& get_category_name() const;

    /** Gets the working verbosity **/
    const LogVerbosity& get_verbosity() const;

    /** Should not generally be used directly. Tests the runtime verbosity and maybe triggers a debug break, etc. **/
    bool is_suppressed(LogVerbosity verbosityLevel) const;

    /** Sets up the working verbosity and clamps to the compile time verbosity. **/
    void set_verbosity(LogVerbosity verbosity);

  private:
    /** Holds the current suppression state **/
    LogVerbosity m_verbosity;

    /** Name for this category **/
    LogCategoryName m_category_name;
  };
} // namespace rex