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

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.add_public_define("REX_USING_DIRECTX");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.LibraryFiles.Add("d3d12.lib");
    conf.LibraryFiles.Add("dxgi.lib");
    conf.LibraryFiles.Add("d3dcompiler.lib");
    conf.LibraryFiles.Add("dxguid.lib");

    conf.AddPublicDependency<RexStd>(target);
    conf.AddPublicDependency<RexRendererCore>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexPIX>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);

    switch (target.Config)
    {
      case Config.debug:
      case Config.debug_opt:
      case Config.coverage:
      case Config.sanitization:
        conf.add_public_define("REX_ENABLE_DX_CALL");
        conf.add_public_define("REX_ENABLE_DX12_DEBUG_LAYER");
        conf.add_public_define("REX_ENABLE_IMGUI");
        conf.add_public_define("REX_ENABLE_DEBUG_RESOURCE_NAMES");
        conf.add_public_define("REX_ENABLE_GFX_DEBUGGING");
        conf.add_public_define("REX_ENABLE_DXGI_DEBUG_LAYER");
        conf.add_public_define("REX_ENABLE_DEBUG_SHADER_COMPILATION");
        break;
      case Config.release:
        break;
      default:
        break;
    }
  }
}