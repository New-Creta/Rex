using Sharpmake;
using System.IO;

[Generate]
public class Regina : ToolsProject
{
  public Regina() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "Regina";
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
    
    string ThisFileFolder = Path.GetFileName(Path.GetDirectoryName(Utils.CurrentFile()));
    conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
    conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = Path.Combine(Globals.Root, "data", ThisFileFolder);
    if (!Directory.Exists(conf.VcxprojUserFile.LocalDebuggerWorkingDirectory))
    {
      Directory.CreateDirectory(conf.VcxprojUserFile.LocalDebuggerWorkingDirectory);
    }

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
