// NOLINTBEGIN

#define CATCH_CONFIG_RUNNER
#include "rex_unit_test/catch2/catch.hpp"

#define REX_CATCH_REGISTER_REPORTER(name, reporterType)                                                                                                                                                                                                \
        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION                                                                                                                                                                                                        \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS                                                                                                                                                                                                         \
        namespace                                                                                                                                                                                                                                        \
        {                                                                                                                                                                                                                                                \
          Catch::ReporterRegistrar<Catch:: reporterType> catch_internal_RegistrarFor##reporterType(name);                                                                                                                                                        \
        }                                                                                                                                                                                                                                                \
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

REX_CATCH_REGISTER_REPORTER("console", ConsoleReporter)

int main(int argc, char* argv[])
{
  return Catch::Session().run(argc, argv);
}

// NOLINTEND
