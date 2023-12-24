using Sharpmake;
using System.IO;

[Generate]
public class ConsoleApp : ToolsProject
{
  public ConsoleApp() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "ConsoleApp";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    base.SetupOutputType(conf, target);

    conf.Output = Configuration.OutputType.Exe;
  }

  protected override void SetupPlatformRules(RexConfiguration conf, RexTarget target)
  {
    base.SetupPlatformRules(conf, target);

    switch (target.Platform)
    {
      case Platform.win32:
      case Platform.win64:
        conf.AddPublicDependency<RexWindows>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
        break;
    }
  }
}
