using Sharpmake;
using System.IO;

[Generate]
public class RexDirectX : PlatformProject
{
  public RexDirectX() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexDirectX";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.LibraryFiles.Add("d3d12.lib");
    conf.LibraryFiles.Add("dxgi.lib");

    conf.AddPublicDependency<RexRendererCore>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);

  }
}