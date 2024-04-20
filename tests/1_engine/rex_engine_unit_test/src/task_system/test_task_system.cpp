#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/task_system/task_system.h"

#include "rex_engine/engine/types.h"
#include "rex_std/atomic.h"

TEST_CASE("Task System")
{
  rsl::atomic<s32> x = 0;
  
  auto t1 = rex::run_async([&x]() { x += 1; }); // 1
  auto t2 = rex::run_async([&x]() { x += 1; }); // 2
  auto t3 = rex::run_async([&x]() { x += 1; }); // 3
  auto t4 = rex::run_async([&x]() { x += 1; }); // 4
  auto t5 = rex::run_async([&x]() { x += 1; }); // 5

  t1.wait_for_me();
  t2.wait_for_me();
  t3.wait_for_me();
  t4.wait_for_me();
  t5.wait_for_me();

  REX_CHECK(x == 5);
}