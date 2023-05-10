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

    string relative_source_path = Util.PathGetRelative(Path.Combine(Globals.SourceRoot), SourceRootPath);
    GenerationConfigPath = Path.Combine(Globals.Root, "config", relative_source_path, "generation.json");
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.Output = Configuration.OutputType.Exe;
    conf.Options.Add(Options.Vc.Linker.SubSystem.Console);

    string ThisFileFolder = Path.GetFileName(Path.GetDirectoryName(Utils.CurrentFile()));
    conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
    conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = Path.Combine(Globals.Root, "data", ThisFileFolder);

    switch (target.Platform)
    {
      case Platform.win32:
      case Platform.win64:
        conf.AddPublicDependency<RexWindows>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
        break;
    }
  }
}
