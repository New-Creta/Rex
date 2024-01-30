# Build Pipeline

## Introduction
Rex's entire building pipeline, going from setting up the initial setup to the actual build process, generation and testing is written in python. This is done to allow quick iteration which would be more of hassle in C# or C++.

Every system in the pipeline has the same interface which is the _rex.py python script, sitting at the root of the project. This script will call into various script that are stored internally. These reason for this is to keep the root of the project as minimalistic as possible.
Each script that the root script calls is only supposed to launch multiple intermediate steps and not do much work on its own.

Most build scripts have a `-clean` command which pretends the script is run for the very first time. This just deletes the output folders before running the script.

The intermediate directory for everything related to rex is the `.rex` folder sitting in the root of the project. This acts similar to `.git`, `.vs` and `.vscode` which are intermediate folders of other programs.
This is done so that it's very easy to start the project from scratch, just delete the `.rex` folder and you're developing again as if it was the very first time.

## Setup

### Root script
To setup Rex Engine after an initial clone from [github](https://github.com/New-Creta/Rex) run [`_rex.py setup`](../../../_rex_.py)

This setup script will install `rexpy` aka `regis` on your machine, after which it'll call the actual setup script located in [`_build/scripts/setup.py`](../../../build/scripts/setup.py) which will continue setting up your machine for rex engine development.

### Setup script

This is the actual setup script that goes through the full setup pipeline
[`setup.py`](../../../_builds/scripts/setup.py) will read some configuration files, looking for dependencies needed for by Rex Engine based on your platform (only Windows is support at time of writing)
These dependencies are found in the various config json files located in `_build/config`.

All installations will add a version file to the dependency that's installed to cache which one you downloaded. This is to avoid redownloading already installed dependencies, which allows us to use an incremental setup.

there are 4 json files located in this folder
### [**settings.json**](../../../_build/config/settings.json)
This is data file containing settings for various stages in the build pipeline and is not just used by the setup process. it holds information how the intermediate folder is called, build folder, source folder, ...

This is done so that we don't hardcode these paths in various different tools. By keeping them in this file and using a well known format (json format) it's very easy to access this data, in any tool (using any language).

### [**required_libs.json**](../../../_build/config/required_libs.json)
This is a data file containing information which libraries are required to develop rex on your platform. eg

If these dependencies aren't found, it'll download these over the network and cache their install paths. (eg. Windows -> Windows SDK).

### [**required_tools.json**](../../../_build/config/required_tools.json)
Similar to [`required_libs.json`](../../../_build/config/required_libs.json) this file contains information which tools are required to develop rex on your platform.

Some examples for these tools are:
- compiler
- linker
- ninja
- sharpmake
- ...

More on these tools later.

### [**required_externals.json**](../../../_build/config/required_externals.json)
For those familiar with Git, this file acts as a description file for the submodules of Rex. 

The reason why they're not stored in a submodule file using Git functionality to clone these dependencies is because it's much faster to store these dependencies in a repository somewhere and manually track which version is needed by your branch.
The added bonus is that we don't have to deal with modules depending on each other, making them harder to track in version control.

Some Examples (Windows):
```sh
# Will do an initial setup.
# Updating non-cached paths and cached paths that no longer exist
py _rex.py setup

# Same as initial setup but will clean intermediates first
py _rex.py setup -clean
```

## Generation
Rex Engine uses a custom [Sharpmake](https://github.com/RisingLiberty/Sharpmake) for its generation. 

### Sharpmake
[Sharpmake](https://github.com/ubisoft/Sharpmake/wiki) is a build tool, written in C# and who's build scripts are also written in C#. We chose sharpmake as the build tool for Rex due to its extended debug possibilities. Because the build scripts themselves are written in C# this allows you to debug them in an IDE with breakpoints, printing, callstack tracing, ...

Another benefit of using C# is it allows you to run actual C# code while you're generating, giving almost limitless possibilities of what you do during generation, this is much harder to do in other build tools (eg. CMAKE, Premake, NMake, ..)

#### **Rex Additions**
The vanilla version of Sharpmake is very powerful, it is however mainly used for `Visual Studio` which is very focussed on `Windows`. It does support `XCode` projects among other things, but we didn't want to have different project files on different platforms. This would also make CI much harder. 

That's why we added support for `Ninja` to Sharpmake.

### [**Ninja**](https://ninja-build.org)
*Ninja is a small build system with a focus on speed*

We use ninja files for building the engine as those files are cross platform.
Vanilla sharpmake doesn't support ninja however, so this was added.

### **Unity Builds**
Also known as combi builds and jumbo builds.

Note: this has nothing to do with the Unity game engine.

A unity build allows the build system to merge source files together, creating 1 big source file. This is simply done with C++ include statements including different .cpp files into intermediate .cpp files. 

These intermediate files are then passed on to the compiler which improves compilation times a lot as the compiler doesn't have to open as many files and can parse more files in 1 go.

Note: Files modified on disk, are excluded from unity files to improve compilation. They also have optimizations disabled so it's easier to debug them.

### Generation Script
The generate script itself is just a wrapper around a sharpmake call.

It'll scan for all sharpmake files recursively in 3 different folders
- build folder (this holds the core scripts used by all the other scripts)
- source folder (each project has at least 1 sharpmake script in its root)
- test folder (each project has at least 1 sharpmake script in its root)

all these files are then passed on to sharpmake and executed.

Because the generation script is a wrapper around a sharpmake call, the script itself doesn't own the commandline arguments, these are owned by Sharpmake.
We needed a way to let sharpmake communicate with the python script and back so that we don't have to maintain the same list of commandlines twice, once in sharpmake and once in the python script. We've achieved this by creating a `default_config.json` file that lives next to the sharpmake build sources

This config file is parsed by the python script and it'll create commandline argument for them. the script will create a `config.json` file in the intermediate build folder which is passed to sharpmake. It'll read the config file and use it to initialize the generation pipeline.

There's only 1 extra commandline argument supported by the generate script which is `-sharpmake_args`. This is to allow a user to pass in arguments that are supported by the native sharpmake executable

We support Visual Studio and Visual Studio Code as IDEs. These just act as text editors as they in turn call into the build pipeline and build the ninja files which are always generated.

Some Examples (Windows):
```sh
# Default generation. generate files for engine and editor (~/_build/sharpmake/data/default_config.json)
py _rex.py generate

# List all the possible configuration settings, no generation gets performed.
py _rex.py generate -h

# Default generation + unit tests
py _rex.py generate -enable-unit-tests 

# Default generation + unit tests + code coverage
py _rex.py generate -enable-unit-tests -enable-code-coverage 

# Default generation + unit tests + code coverage + Visual Studio sln and projects
py _rex.py generate -enable-unit-tests -enable-code-coverage -IDE VisualStudio19

# Default generation, but single threaded
py _rex.py generate -sharpmake_args /multithreaded(false)
```

### Code Generation
The generation step also generates code. The settings for code generation for a particular project are located in the `config` folder and follow the same folder structure as their source root directory. These settings indicate what gets generated and where it gets generated.

Sharpmake supports enum and array generation.

A code generation object always has a `key`. multiple projects can have an object with the same key and it's `content` will be merged together in the final source file.
A key should have 1 and only 1 owner. An owner is a project's code generation object. This owner needs to specify the following
- `Type` - What are we generating? can be `Enum` or `Array`
- `Filepath` - This is the file where the code will be generated in

for `Enums` the following needs to be specified
- `ClassName` - This is the name of the enum class to be generated

for `Arrays` the following needs to be specified
- `Name` - This is the name of the variable that'll hold the content of the array
- `ElementType` - This is the type the array will hold
- `Includes` - This is the files that need to be included at the top of the file

## Building
Building through Visual Studio and Visual Studio Code works the same as you would otherwise. We leverage the IDE's ability to run commandlines as build steps.
These commandlines call into the actual build script which perform a build of a project in a specified configuration using a specified compiler.

As mentioned above, the build script takes in the project, configuration and compiler you want to build with. It'll call the ninja files and their dependencies (if specified) and creates their output targets.

After each build the `build_projects.json` file in the intermediate build folder is updated. This json file acts as a cache for the launch script so it knows what it can launch.

### Build Types
We have a few different kind of build types aka configurations, some are only supported with Clang as they are sanitizers.
The different builds types are

- Debug: No optimizations enabled. All debug information is available.
- Debug Opt: Optimized version of Debug build.
- Release: All optimizations enabled, no debug information is available.
- Coverage: Only enabled for Clang. Enables code coverage reports. Detects which code gets run at runtime, the reports get generate in the intermediate folder of your project
- [Address Sanitizer](https://clang.llvm.org/docs/AddressSanitizer.html): Only enabled for Clang. Used to detect out of bounds reads/writes. Double frees, use after free.
- [Undefined Behavior Sanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html): Only enabled for Clang. Used to detect undefined behavior at runtime.
- [Fuzzy](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html): Only enabled for Clang. Used for fuzzing projects

Some Examples:
```sh
# Builds regina project and its dependencies in debug mode using msvc compiler
py _rex.py build -project=regina -config=debug -compiler=msvc

# Cleans all regina and its depedencies' intermediate files and then performs a build afterwards
py _rex.py build -project=regina -config=debug -compiler=msvc -clean
```

### Post Build
the sharpmake scripts for all rex related projects launch the [`post_build.py`](../../../source/post_build.py) script found at the source root.
This is for internal use only.

This script launches clang-tidy and clang-format if clang tools are enabled. Because clang-tidy is run using clang, the post build is only enabled when compiling with clang

After which it launches a script called `post_build_by_user.py`, if it's found at the root of your project's source code, which the user can write to write their own post build events

## Testing
Rex Engine supports many different kind of tests for optimal testing which can all be fired from the root script

To run all tests, pass in `-all` to `_rex.py test`

The following tests are supported for Rex Engine

### include-what-you-use
flag: `-iwyu`

A tool providing directly hooked into the compiler to scan for includes that aren't needed and could be replaced with a forward declaration or removed all together.
### clang-tidy
flag: `-clang_tidy`

A static analyser directly hooked into the compiler to scan for possible design problems, readability issues or bugs. None of the checks in here will auto fix themselves and all warnings are treated as error.
### Unit tests
flag: `-unit_tests`

Every big framework should have its own unit tests so of course rex has it own as well. We use catch2 for our unit tests, these unit tests can be added to the visual studio solution by passing in `-generate_unittests` to [`_generate.py`](../../../_generate.py)
### Coverage
flag: `-coverage`

Code coverage is a good way to verify all your code is executed and you don't have any deadcode anywhere in your codebase. With a big framework it becomes tricky to spot which code is still active and which code can be removed, code coverage is implemented to resolve and quickly find dead code in your code base.
With this flag, unit tests will run with coverage enabled. A coverage html report is auto generated after each run allowing you to inspect which code has been executed and which code hasn't.
### Address Sanitizer
flag: `-asan`

Address sanitizer is a flag added to the compiler to add extra code with memory access instructions. This is useful to track down memory issues you might not even know you have. With this flag, unit tests will be run with asan enabled, tracking down if you have any memory bugs in your code.
### Undefined Behavior Sanitizer
flag: `-ubsan`

Undefined behavior sanitizer is a similar flag as address sanitizer but instead looking for memory bugs, it looks for undefined behavior in your code. with this flag, unit tests will be run with ubsan enabled, tracking down if you have any undefined behavior in your code. 
### Fuzzy Testing
flag: `-fuzzy`

Fuzzy Testing is a special kind of testing where the program will pass in a new buffer with random length and random data to your program over and over again. It's up to you to then cast this data into some result and run it on your code, checking if you're can handle random (and wrong input). it's highly recommended to enable this with asan and ubsan enabled as well, checking if you're code doesn't run into any memory or undefined issues.
With this flag enabled, you'll run the fuzzy tests for the Rex Engine.

Some Examples:
```sh
# This will run all the test of Rex Engine
py _rex.py test -all

# This will run include-what-you-use on the Rex codebase
py _rex.py test -iwyu

# This will run include-what-you-use and clang-tidy on the Rex codebase
py _rex.py test -iwyu -clang_tidy

# This will run the unit tests of the Rex codebase
py _rex.py test -unit_tests

# This will run address sanitizer and undefined behavior on the unit tests of Rex codebase
py _rex.py test -asan -ubsan

# This will run the fuzzy tests and unit tests of the Rex codebase
py _rex.py test -fuzzy -unit_tests

```