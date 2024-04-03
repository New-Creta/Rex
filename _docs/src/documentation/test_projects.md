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

[`Unit Test Sharpmake Example`](../../../_build/sharpmake/templates/rex_unit_test_template.sharpmake.cs)

Lastly, make sure you add it to the solution in `sln.cs` by using `conf.AddProject<RexEngineUnitTests>(target);`
That's all you need to setup a unit test project, all that's left is to add your first source file with a unit test.
For every source file that needs to reference catch2, add an include to `rex_unit_test/rex_catch2.h`

[`Unit Test Example`](../../../_build/sharpmake/templates/rex_unit_test_template.cpp)

#### Commandline Project Generation

There's a quick way of generating a unit test project by using the rex pipeline. The following command will generate a new unit test project called `MyRexUnitTestProject` and put it under the `1_engine` solution folder

`py _rex.py unit-test -create -project=MyRexUnitTestProject -solution_folder=1_engine`

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

[`Auto Test Sharpmake Example`](../../../_build/sharpmake/templates/rex_auto_test_template.sharpmake.cs)

Next you need to create a source file which will act as the entry point of the auto tests, let's call this file `regina_auto_tests.cpp`
Fill the file with the following code

[`Auto Test Entrypoint Example`](../../../_build/sharpmake/templates/rex_auto_test_entry_template.cpp)

This will add the boilerplate code that's needed to start the auto tests.
We added a boot auto test example here, let's dig in to how you'd implement that.

Create a file called `regina_boot_test.cpp`

We need to fill it in with the same boiler plate as a regular app that uses the rex engine.
We define an `initialize`, `update`, and `shutdown` function, which we pass over to the engine

[`Auto Test Example`](../../../_build/sharpmake/templates/rex_auto_test_template.cpp)

That's all there is to it, once you have this code, you can run it and the editor boot will now be tested.
If any crashes occur this will be reported by the testing pipeline

#### Commandline Project Generation

There's a quick way of generating a auto test project by using the rex pipeline. The following command will generate a new auto test project called `MyRexAutoTestProject` and put it under the `1_engine` solution folder

`py _rex.py auto-test -create -project=MyRexUnitTestProject -solution_folder=1_engine`

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

[`Fuzzy Test Sharpmake Example`](../../../_build/sharpmake/templates/rex_fuzzy_test_template.sharpmake.cs)

Next you need to create a source file which will act as the entry point of the fuzzy tests, let's call this file `rex_engine_fuzzy_tests.cpp`
Fill the file with the following code

[`Fuzzy Test Example`](../../../_build/sharpmake/templates/rex_fuzzy_test_template.cpp)

And that's it. You can run any fuzzy tests in the entry, divide it up into different function calls, etc..
The goal is that you call as many systems as possible, you don't know the input you're giving them, the goal is just that they don't crash.

#### Commandline Project Generation
There's a quick way of generating a fuzzy test project by using the rex pipeline. The following command will generate a new fuzzy test project called `MyRexFuzzyTestProject` and put it under the `1_engine` solution folder

`py _rex.py fuzzy-test -create -project=MyRexUnitTestProject -solution_folder=1_engine`
