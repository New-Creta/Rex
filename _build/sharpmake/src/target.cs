using System.Linq;
using System.Collections.Generic;
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
      string config = string.Concat(Config.ToString().Select((x, i) => i > 0 && char.IsUpper(x) ? "_" + x.ToString() : x.ToString()));
      string comp = Compiler.ToString();

      string name = string.Concat(config, " - ", comp);
      return name;
    }
  }

  public override string ProjectConfigurationName
  {
    get
    {
      string config = string.Concat(Config.ToString().Select((x, i) => i > 0 && char.IsUpper(x) ? "_" + x.ToString() : x.ToString()));
      return config;
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

  public static List<RexTarget> CreateTargets()
  {
    List<RexTarget> targets = new List<RexTarget>();

    // Always add the ninja target. Ninja is our main build system and is used what gets used by the rex development pipeline
    targets.AddRange(CreateTargetsForDevEnv(DevEnv.ninja));

    switch (ProjectGen.Settings.IDE)
    {
      case ProjectGen.IDE.VisualStudio:
        if (Util.GetVisualStudioInstallationsFromQuery(DevEnv.vs2019).Count > 0)
        { 
          targets.AddRange(CreateTargetsForDevEnv(DevEnv.vs2019));
        }
        else
        {
          System.Console.WriteLine("Visual Studio IDE specified, but it's not installed");
        }
        break;
      case ProjectGen.IDE.VSCode:
        targets.AddRange(CreateTargetsForDevEnv(DevEnv.vscode));
        break;
      default:
        break;
    }

    return targets;
  }

  public static List<RexTarget> CreateTargetsForDevEnv(DevEnv devEnv)
  {
    List<RexTarget> targets = new List<RexTarget>();

    // The checks specified here are checks for various testing types
    // Thse checks do not work with Visual Studio and are only supported through the rex pipeline.
    if (ProjectGen.Settings.CoverageEnabled)
    {
      targets.Add(new RexTarget(Platform.win64, devEnv, Config.coverage, Compiler.Clang));
    }
    if (ProjectGen.Settings.AsanEnabled)
    {
      targets.Add(new RexTarget(Platform.win64, devEnv, Config.address_sanitizer, Compiler.Clang));
    }
    if (ProjectGen.Settings.UbsanEnabled)
    {
      targets.Add(new RexTarget(Platform.win64, devEnv, Config.undefined_behavior_sanitizer, Compiler.Clang));
    }
    if (ProjectGen.Settings.FuzzyTestingEnabled)
    {
      targets.Add(new RexTarget(Platform.win64, devEnv, Config.fuzzy, Compiler.Clang));
    }
    
    targets.Add(new RexTarget(Platform.win64, devEnv, Config.debug | Config.debug_opt | Config.release, Compiler.MSVC | Compiler.Clang));

    return targets;
  }

}

