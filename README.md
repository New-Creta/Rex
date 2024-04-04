# REX

Image here ...

Rex is Game Engine is an 3D game engine written in C++. It's still in its early stages of development so almost everything is up for change in the future. The goal of rex is to implement a AAA game engine with a focus on safety, scale and performance. The goal is to develop games efficiently and safely, with clear visibility on dependencies and having testing (unit testing, automation and fuzzy testing) available to test any changes to a project.



## Getting Started

there's no prerequisites for Rex other than having python installed.

<ins>Downloading the repository:</ins>

1. Start by cloning with `git clone https://github.com/New-Creta/Rex`
2. Run `py _rex.py setup` in the root directory
3. Run `py _rex.py generate` to generate the visual studio solution (and Ninja files).
4. Run `py _rex.py build` to build the engine and editor
5. Run `py _rex.py launch` to launch the editor

More documentation on the rex pipeline can be found [here](_docs/src/documentation/rex_pipeline.md)

<ins>Configuring the project:</ins>

Rex uses sharpmake as it's build tool. Sharpmake is chosen as it's using C#, so there's no need to write special debugging tools as you have all of C#'s tooling available tool you, including the debugger.

A project's sharpmake file is always found at the root of the source files. (eg. [Engine sharpmake file](source/1_engine/rex_engine/rex_engine.sharpmake.cs))

## Development 

Dave De Breuck a.k.a. REX1

3D Developer @ Twikit

davedebreuck.gamedeveloper@gmail.com

Nick De Breuck a.k.a. REX2

Engine Programmer @ Rocksteady

nick.debreuck@outlook.com


## Standard Library

Rex has developed its own standard library. This is to step away from the C++ standard library due to its performance implications and backwards compatibility problems. The standard library of Rex is standalone and be used within any project, therefore it's a standalone github repository which can be found [here](https://github.com/RisingLiberty/RexStl)

