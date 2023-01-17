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
    get { return ConfigManager.get_optimization_for_config(Config); }
  }

  public static RexTarget[] GetDefaultTargets()
  {
    return new RexTarget[]
    { 
      GetVsOnlyTarget(),
      new RexTarget(Platform.win64, DevEnv.ninja, Config.debug | Config.debug_opt | Config.release, Compiler.MSVC | Compiler.Clang)
    };
  }
  public static RexTarget GetVsOnlyTarget()
  {
    return new RexTarget(Platform.win64, DevEnv.vs2019, Config.debug | Config.debug_opt | Config.release, Compiler.MSVC);
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

