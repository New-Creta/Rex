using Sharpmake;
using System.IO;

[Generate]
public class RexApplicationCore : AppLibrariesProject
{
  public RexApplicationCore() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexApplicationCore";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexWindows>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
  }
}
