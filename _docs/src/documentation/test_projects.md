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

That's all you need to setup a unit test project, all that's left is to add your first source file with a unit test.
For every source file that needs to refernce catch2, add an include to `rex_test/rex_catch2.h`

```cpp
#include "rex_test/rex_catch2.h"

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



### Fuzzy Test Project