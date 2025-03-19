// NOLINTBEGIN

#include "rex_engine/engine/globals.h"

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

class RextTestRunListener : public Catch::TestEventListenerBase {
public:
  RextTestRunListener(const Catch::ReporterConfig& _config)
    : Catch::TestEventListenerBase(_config)
    , m_test_frame_based_allocator(nullptr)
  {}

  void testCaseStarting(Catch::TestCaseInfo const&) override
  {
    if (rex::globals().single_frame_allocator == nullptr)
    {
      auto frame_based_allocator = rsl::make_unique<rex::FrameBasedAllocator>(1_mib, 1);
      m_test_frame_based_allocator = frame_based_allocator.get();
      rex::set_single_frame_alloc(rsl::move(frame_based_allocator));
    }
    else if (m_test_frame_based_allocator)
    {
      m_test_frame_based_allocator->adv_frame();
    }
  }

private:
  rex::FrameBasedAllocator* m_test_frame_based_allocator;
};
Catch::ListenerRegistrar<RextTestRunListener> test_event_listener;

CATCH_REGISTER_LISTENER(RextTestRunListener)

int main(int argc, char* argv[])
{
  return Catch::Session().run(argc, argv);
}

// NOLINTEND
