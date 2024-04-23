using Sharpmake;
using System.IO;

[Generate]
public class RexDXGI : PlatformProject
{
  public RexDXGI() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexDXGI";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.LibraryFiles.Add("dxgi.lib");
    conf.LibraryFiles.Add("dxguid.lib");

    conf.AddPublicDependency<RexStd>(target);
    conf.AddPublicDependency<RexRendererCore>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);

    switch (target.Config)
    {
      case Config.debug:
      case Config.debug_opt:
      case Config.coverage:
      case Config.sanitization:
        conf.add_public_define("REX_ENABLE_DX_CALL");
        break;
      case Config.release:
        break;
      default:
        break;
    }
  }
}