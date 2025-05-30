using Sharpmake;
using System.IO;

[Generate]
public class HttpServer : ToolsProject
{
  public HttpServer() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "HttpServer";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<HttpShared>(target);
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
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

        if (target.Config == Config.release)
        {
          conf.add_public_define("REX_WINDOWS_GUI_APP");
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
