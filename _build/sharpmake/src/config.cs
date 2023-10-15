using Sharpmake;
using System.Collections.Generic;

[Fragment, System.Flags]
public enum Config
{
  assert = (1 << 0),
  debug = (1 << 1),
  debug_opt = (1 << 2),
  release = (1 << 3),
  tests = (1 << 4),
  coverage = (1 << 5),
  address_sanitizer = (1 << 6),
  undefined_behavior_sanitizer = (1 << 7),
  fuzzy = (1 << 8)
}

[Fragment, System.Flags]
public enum Optimization
{
  NoOpt = (1 << 0),
  FullOptWithPdb = (1 << 1),
  FullOpt = (1 << 2)
}

public class RexConfiguration : Sharpmake.Project.Configuration
{
  public bool GenerateCompilerDB { get; set; }

  public RexConfiguration()
  {
    GenerateCompilerDB = true;
  }

  public void enable_exceptions()
  {
    Options.Remove(Sharpmake.Options.Vc.Compiler.Exceptions.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);

    // Requirement for MSVC
    Defines.Remove("_HAS_EXCEPTIONS=0");
    Defines.Add("_HAS_EXCEPTIONS=1");
  }

  public void disable_exceptions()
  {
    Options.Remove(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Disable);

    // Requirement for MSVC
    Defines.Remove("_HAS_EXCEPTIONS=1");
    Defines.Add("_HAS_EXCEPTIONS=0");
  }

  public void add_public_define(string define)
  {
    Defines.Add(define);
    ExportDefines.Add(define);
  }

  public void set_precomp_header(string projectFolderName, string preCompHeaderName)
  {
    PrecompHeader = projectFolderName + @"/" + preCompHeaderName + @".h";
    PrecompSource = preCompHeaderName + @".cpp";
  }

  public bool is_config_for_testing()
  {
    RexTarget rex_target = Target as RexTarget;
    switch (rex_target.Config)
    {
      case Config.tests:
      case Config.coverage:
      case Config.address_sanitizer:
      case Config.undefined_behavior_sanitizer:
      case Config.fuzzy:
        return true;

      case Config.assert:
      case Config.debug:
      case Config.debug_opt:
      case Config.release:
      default:
        return false;
    }
  }
}

public class ConfigManager
{
  public static Config get_all_configs()
  {
    return Config.assert | Config.debug | Config.debug_opt | Config.release | Config.tests;
  }

  public static Optimization get_optimization_for_config(Config config)
  {
    switch (config)
    {
      case Config.assert: return Optimization.FullOptWithPdb;
      case Config.debug: return Optimization.NoOpt;
      case Config.coverage: return Optimization.NoOpt;
      case Config.debug_opt: return Optimization.FullOptWithPdb;
      case Config.address_sanitizer: return Optimization.FullOptWithPdb;
      case Config.undefined_behavior_sanitizer: return Optimization.FullOptWithPdb;
      case Config.fuzzy: return Optimization.FullOptWithPdb;
      case Config.release: return Optimization.FullOpt;
      case Config.tests: return Optimization.NoOpt;
    }
    return Optimization.FullOpt;
  }
}

public class ConfigSetting
{
  public string Description { get; set; }
  public System.Text.Json.JsonElement Value { get; set; }
  public string[] Options { get; set; }
}