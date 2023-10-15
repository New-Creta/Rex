using Sharpmake;
using System.Collections.Generic;

// This file describes the classes used to configuring projects.
// A configuration is often tied to optimisation levels
// That's why the optimisation level is described here as well.

// For documentation about each configuraton
// Please read _docs/books/build_pipeline.html
[Fragment, System.Flags]
public enum Config
{
  assert = (1 << 0),
  debug = (1 << 1),
  debug_opt = (1 << 2),
  release = (1 << 3),
  coverage = (1 << 4),
  address_sanitizer = (1 << 5),
  undefined_behavior_sanitizer = (1 << 6),
  fuzzy = (1 << 8)
}

// High level different kind of optimizations we support
// Compiler and Linker options are set based on these values
[Fragment, System.Flags]
public enum Optimization
{
  NoOpt = (1 << 0),
  FullOptWithPdb = (1 << 1),
  FullOpt = (1 << 2)
}

// The RexConfiguration class is what's used to set configuration options
// These can be optimisation level, include paths, dependencies, ..
public class RexConfiguration : Sharpmake.Project.Configuration
{
  // Enables exceptions for this configuration
  public void enable_exceptions()
  {
    Options.Remove(Sharpmake.Options.Vc.Compiler.Exceptions.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);

    // Requirement for MSVC
    Defines.Remove("_HAS_EXCEPTIONS=0");
    Defines.Add("_HAS_EXCEPTIONS=1");
  }

  // Disables exceptions for this configuration
  public void disable_exceptions()
  {
    Options.Remove(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Disable);

    // Requirement for MSVC
    Defines.Remove("_HAS_EXCEPTIONS=1");
    Defines.Add("_HAS_EXCEPTIONS=0");
  }

  // Adds a define to this configuration which is propagated
  // to other projects that have a dependency on this project
  // and use this configuration for it as well.
  public void add_public_define(string define)
  {
    Defines.Add(define);
    ExportDefines.Add(define);
  }
}

// Every entry in the config file that's passed in to a call into sharpmake
// is deserialized into this structure.
// This is then used to initialize the generation settings
public class ConfigSetting
{
  public string Description { get; set; }
  public System.Text.Json.JsonElement Value { get; set; }
  public string[] Options { get; set; }
}