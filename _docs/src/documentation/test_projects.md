# How To Create Test Projects

## Type Of Test Project

Rex supports 3 different kind of test projects, each requiring a slightly different setup.

### Unit Test Project
A unit test project is ideal to test a library or system that doesn't need to engine to be in a specific state. If you have a container, allocator, algorithm, .. that you'd like to test, a unit test is ideal to do this.

Let's work from an example. Let's pretend we want to create a unit test project for the rex engine project called `RexUnitTests`.
To create a unit test project, create a new folder in ~/tests under the specific subfolder you need, in this case, because we're making a unit test for the engine itself, we'll make a folder under `1_engine`.

Like any other project, we need to create a `src` and `include` folder and a `sharpmake file`.

this would give us the following folder structure
- ~/tests/1_engine/rex_unit_tests/include
- ~/tests/1_engine/rex_unit_tests/src
- ~/tests/1_engine/rex_unit_tests/rex_unit_tests.sharpmake.cs

if needed, you also need to create a code generation config file under `~/config/1_engine/rex_unit_tests`. For this example, we don't need such a file.

Next you need to configure the sharpmake project. You do this like you'd configure any other project, but every unit test project needs 2 things to work properly.
- The `ProjectType` needs to be specified in the constructor. Because we're generating a unit test, specify `ProjectType = ProjectGen.TestProjectType.UnitTest;`.
- We need to add a dependency to `RexUnitTest` project. Override the `SetupLibDependencies` func and add a public dependency to `RexUnitTest`.

An example of such a file could look like this

```cs
using Sharpmake;
using System.IO;

[Generate]
public class RexEngineUnitTests : TestProject
{
  public RexEngineUnitTests() : base()
  {
    Name = GenerateName("RexEngineUnitTests");
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    ProjectType = ProjectGen.TestProjectType.UnitTest;
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    base.SetupSolutionFolder(conf, target);

    conf.SolutionFolder = Path.Combine(conf.SolutionFolder, "1_engine");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexUnitTest>(target, DependencySetting.Default);
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.Options.Remove(Options.Vc.Compiler.JumboBuild.Enable);
  }
}
```

Lastly, make sure you add it to the solution in `sln.cs` by using `conf.AddProject<RexEngineUnitTests>(target);`
That's all you need to setup a unit test project, all that's left is to add your first source file with a unit test.
For every source file that needs to reference catch2, add an include to `rex_unit_test/rex_catch2.h`

```cpp
#include "rex_unit_test/rex_catch2.h"

TEST_CASE("My Test Case")
{
    // Test Code Here
}
```

### Auto Test Project
An auto test is ideal to test a complex system or test different systems working together. It'll launch an app, backed by the rex engine, in such a way that no user input is needed.

Just like before, let's work from an example. Let's pretend we want to create a auto test project for the editor called `ReginaAutoTests`
To create a auto test project, create a new folder in ~/tests under the specific subfolder you need, in this case, because we're making an auto test for the editor, we'll make a folder under `4_tools`.

Like any other project, we need to create a `src` and `include` folder and a `sharpmake file`.

this would give us the following folder structure
- ~/tests/4_tools/regina_auto_tests/include
- ~/tests/4_tools/regina_auto_tests/src
- ~/tests/4_tools/regina_auto_tests/regina_auto_tests.sharpmake.cs

if needed, you also need to create a code generation config file under `~/config/4_tools/regina_auto_tests`. For this example, we don't need such a file.

Next you need to configure the sharpmake project. You do this like you'd configure any other project, but auto unit test project needs 2 things to work properly.
- The `ProjectType` needs to be specified in the constructor. Because we're generating a auto test, specify `ProjectType = ProjectGen.TestProjectType.AutoTest;`.
- We need to add a dependency to `RexAutoTest` project. Override the `SetupLibDependencies` func and add a public dependency to `RexAutoTest`.

An example of such a file could look like this
```cs
using Sharpmake;
using System.IO;

[Generate]
public class ReginaAutoTests : TestProject
{
  public ReginaAutoTests() : base()
  {
    Name = GenerateName("ReginaAutoTests");
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    ProjectType = ProjectGen.TestProjectType.AutoTest;
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    base.SetupSolutionFolder(conf, target);

    conf.SolutionFolder = Path.Combine(conf.SolutionFolder, "4_tools");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<Regina>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexAutoTest>(target, DependencySetting.Default);
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.Options.Remove(Options.Vc.Compiler.JumboBuild.Enable);
  }
}
```

Next you need to create a source file which will act as the entry point of the auto tests, let's call this file `regina_auto_tests.cpp`
Fill the file with the following code

```cpp
// Basic functionality to create auto tests
#include "rex_auto_test/auto_test.h"

// Entry point we need to define for the rex engine
#include "rex_engine/entrypoint.h"

// Holds the definition of ApplicationCreationParams
#include "rex_engine/engine_params.h"

// Includes to query and diagnose the auto test to run
#include "rex_engine/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

// Holds the definition of PlatformCreationParams
#include "rex_windows/platform_creation_params.h"

namespace regina
{
  rsl::array g_auto_tests =
  {
    // Declare auto tests here
    // The following is just an example
    rex::auto_test::AutoTest("Boot", regina_auto_test::boot_test_entry)
  };

} // namespace regina

namespace rex
{
  rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams&& platformParams)
  {
    // Get the auto test cmd line argument. This holds the name of the auto test we want to run
    rsl::optional<rsl::string_view> cmdline = rex::cmdline::get_argument("AutoTest");

    // Make sure it's a known auto test
    REX_ASSERT_X(cmdline.has_value(), "Auto test fired but no auto test specified on the commandline. Commandline: {}", rex::cmdline::get());

    // Loop through our known auto tests and launch the one we want
    for (rex::auto_test::AutoTest& auto_test : regina::g_auto_tests)
    {
      if (auto_test.is_enabled(cmdline.value()))
      {
        return auto_test.launch(rsl::move(platformParams));
      }
    }

    // Assert if no valid auto test is found
    REX_ASSERT("No auto test found for {}", cmdline.value());

    // Return invalid app params if no valid auto test is found
    return rex::auto_test::invalid_app_params(rsl::move(platformParams));
  }
} // namespace rex

```

This will add the boilerplate code that's needed to start the auto tests.
We added a boot auto test example here, let's dig in to how you'd implement that.

Create a file called `regina_boot_test.cpp`

We need to fill it in with the same boiler plate as a regular app that uses the rex engine.
We define an `initialize`, `update`, and `shutdown` function, which we pass over to the engine

```cpp

#include "regina_auto_test/regina_boot_test.h"
#include "regina/regina.h"
#include "rex_engine/event_system.h"
#include "rex_windows/platform_creation_params.h"

namespace regina_auto_test
{
  // We simply want to initialize the editor and do nothing else
  bool initialize()
  {
    return regina::initialize();
  }
  void update()
  {
    // On the first tick, we want to send the quit event
    // Booting has finished, we can shut down
    rex::event_system::Event ev{};
    ev.type = rex::event_system::EventType::QuitApp;
    rex::event_system::fire_event(ev);
  }
  void shutdown()
  {
    regina::shutdown();
  }

  // This acts as the entry function for the auto test.
  // We simply fill in the functions of the auto test and return
  rex::ApplicationCreationParams boot_test_entry(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params = regina::create_regina_app_creation_params(rsl::move(platformParams));

    app_params.gui_params.window_title = "Regina Test";

    app_params.engine_params.app_init_func = regina_auto_test::initialize;
    app_params.engine_params.app_update_func = regina_auto_test::update;
    app_params.engine_params.app_shutdown_func = regina_auto_test::shutdown;

    return app_params;
  }
} // namespace regina_auto_test

```

That's all there is to it, once you have this code, you can run it and the editor boot will now be tested.
If any crashes occur this will be reported by the testing pipeline

### Fuzzy Test Project

A fuzzy test is meant to be used to test your systems with random input. Fuzzed apps will run many iterations with garbage input, which can be used to test if systems won't crash or perform invalid operations if a user would put enter unexpected input, that either being invalid input to a text field, invalid key strokes but it could also test invalid parameters passed into a function.

Let's work from an example for this as well. Let's pretend we want to create a fuzzy test project for the engine called `RexEngineFuzzyTests`
To create a fuzzy test project, create a new folder in ~/tests under the specific subfolder you need, in this case, because we're making an fuzzy test for the engine, we'll make a folder under `1_engine`.

Like any other project, we need to create a `src` and `include` folder and a `sharpmake file`.

this would give us the following folder structure
- ~/tests/1_engine/rex_engine_fuzzy_tests/include
- ~/tests/1_engine/rex_engine_fuzzy_tests/src
- ~/tests/1_engine/rex_engine_fuzzy_tests/rex_engine_fuzzy_tests.sharpmake.cs

if needed, you also need to create a code generation config file under `~/config/1_engine/rex_engine_fuzzy_tests`. For this example, we don't need such a file.

Next you need to configure the sharpmake project. You do this like you'd configure any other project, but every fuzzy test project needs 2 things to work properly.
- The `ProjectType` needs to be specified in the constructor. Because we're generating a fuzzy test, specify `ProjectType = ProjectGen.TestProjectType.FuzzyTest;`.
- We need to add a dependency to `RexFuzzyTest` project. Override the `SetupLibDependencies` func and add a public dependency to `RexFuzzyTest`.

An example of such a file could look like this

```cs
using Sharpmake;
using System.IO;

[Generate]
public class RexEngineFuzzyTests : TestProject
{
  public RexEngineFuzzyTests() : base()
  {
    Name = GenerateName("RexEngineFuzzyTests");
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    ProjectType = ProjectGen.TestProjectType.UnitTest;
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    base.SetupSolutionFolder(conf, target);

    conf.SolutionFolder = Path.Combine(conf.SolutionFolder, "1_engine");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexFuzzyTest>(target, DependencySetting.Default);
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.Options.Remove(Options.Vc.Compiler.JumboBuild.Enable);
  }
}
```

Next you need to create a source file which will act as the entry point of the fuzzy tests, let's call this file `rex_engine_fuzzy_tests.cpp`
Fill the file with the following code


```cpp
#include "rex_fuzzy_test/rex_fuzzy_test.h"

#include "rex_std/iostream.h"

namespace rex
{
  namespace fuzzy
  {
    int fuzzy_entry(fuzz_span input)
    {
      int test_val = rex::fuzzy::make_fuzzy_object<int>(input);

      return 0;
    }
  }
}
```

And that's it. You can run any fuzzy tests in the entry, divide it up into different function calls, etc..
The goal is that you call as many systems as possible, you don't know the input you're giving them, the goal is just that they don't crash.