using Sharpmake;
using System.IO;

[Generate]
public class PokemonProject : GameProject
{
  public PokemonProject() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "Pokemon";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    DataPath = Path.Combine(Globals.Root, "data", "pokemon");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    if (ProjectGen.Settings.AutoTestsEnabled)
    {
      conf.Output = Configuration.OutputType.Lib;
      conf.add_public_define("REX_ENABLE_AUTO_TESTS");
    }
    else
    {
      conf.Output = Configuration.OutputType.Exe;
    }
  }

  protected override void SetupPlatformRules(RexConfiguration conf, RexTarget target)
  {
    base.SetupPlatformRules(conf, target);

    switch (target.Platform)
    {
      case Platform.win32:
      case Platform.win64:
        conf.AddPublicDependency<RexWindows>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);

        if (target.Config == Config.release)
        {
          conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
        }
        else
        {
          conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
        }

        break;
    }
  }
}
