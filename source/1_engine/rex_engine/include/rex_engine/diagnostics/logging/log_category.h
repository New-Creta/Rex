#pragma once

#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_std/string.h"
#include "rex_std_extra/utility/yes_no.h"

namespace rex
{
    DEFINE_YES_NO_ENUM(IsAsync);

    using LogCategoryName = rsl::string_view;

    /** Base class for all log categories. **/
    struct LogCategory
    {
        /**
         * Constructor, registers with the log suppression system and sets up the default values.
         * @param CategoryName, name of the category
         * @param InDefaultVerbosity, default verbosity for the category, may ignored and overridden by many other mechanisms
         * @param inAsync, should this Category log async or not.
         **/
        LogCategory(const LogCategoryName& inCategoryName, LogVerbosity inDefaultVerbosity, IsAsync inAsync = IsAsync::no);

        /** Gets the category name **/
        rsl::string_view get_category_name() const;

        /** Gets the working verbosity **/
        LogVerbosity get_verbosity() const;

        /** Should not generally be used directly. Tests the runtime verbosity and maybe triggers a debug break, etc. **/
        bool is_suppressed(LogVerbosity verbosityLevel) const;

        /** Check if this logger is async */
        bool is_async() const;

        /** Sets up the working verbosity and clamps to the compile time verbosity. **/
        void set_verbosity(LogVerbosity verbosity);

    private:
        /** Holds the current suppression state **/
        LogVerbosity m_verbosity;

        /** Name for this category **/
        LogCategoryName m_category_name;

        /** Should this logger be an async logger **/
        IsAsync m_is_async;
    };
} // namespace rex