using System.Linq;
using Sharpmake;

public class RexTarget : ITarget
{
  public DevEnv DevEnv;
  public Platform Platform;
  public Config Config;
  public Compiler Compiler;
  public DotNetFramework DotNetFramework = DotNetFramework.v4_8;
  public RexTarget()
  { }
  public RexTarget(Platform platform, DevEnv devEnv, Config config, Compiler compiler = Compiler.MSVC)
  {
    DevEnv = devEnv;
    Platform = platform;
    Config = config;
    Compiler = compiler;
  }

  // This is the display name of the configuration dropdown in visual studio
  public override string Name
  {
    get
    {
      string config_str = string.Concat(Config.ToString().ToString().Select((x, i) => i > 0 && char.IsUpper(x) ? "_" + x.ToString() : x.ToString()));

      return (config_str).ToLowerInvariant();
    }
  }

  public Optimization Optimization
  {
    get
    {
      switch (Config)
      {
        case Config.assert: return Optimization.FullOptWithPdb;
        case Config.debug: return Optimization.NoOpt;
        case Config.coverage: return Optimization.NoOpt;
        case Config.debug_opt: return Optimization.FullOptWithPdb;
        case Config.address_sanitizer: return Optimization.FullOptWithPdb;
        case Config.undefined_behavior_sanitizer: return Optimization.FullOptWithPdb;
        case Config.fuzzy: return Optimization.FullOptWithPdb;
        case Config.release: return Optimization.FullOpt;
      }
      return Optimization.FullOpt;
    }
  }

  public static RexTarget[] GetAllDefaultTargets()
  {
    return new RexTarget[]
    {
      GetVSOnlyTarget(),
      GetNinjaOnlyTarget()
    };
  }
  public static RexTarget GetVSOnlyTarget()
  {
    return new RexTarget(Platform.win64, DevEnv.vs2019, Config.debug | Config.debug_opt | Config.release, Compiler.MSVC);
  }

  public static RexTarget GetNinjaOnlyTarget()
  {
    return new RexTarget(Platform.win64, DevEnv.ninja, Config.debug | Config.debug_opt | Config.release, Compiler.MSVC | Compiler.Clang);
  }

  public static RexTarget GetCoverageTarget()
  {
    return new RexTarget(Platform.win64, DevEnv.ninja, Config.coverage, Compiler.Clang);
  }

  public static RexTarget GetAsanTarget()
  {
    return new RexTarget(Platform.win64, DevEnv.ninja, Config.address_sanitizer, Compiler.Clang);
  }
  public static RexTarget GetUBsanTarget()
  {
    return new RexTarget(Platform.win64, DevEnv.ninja, Config.undefined_behavior_sanitizer, Compiler.Clang);
  }
  public static RexTarget GetFuzzyTarget()
  {
    return new RexTarget(Platform.win64, DevEnv.ninja, Config.fuzzy, Compiler.Clang);
  }

}

