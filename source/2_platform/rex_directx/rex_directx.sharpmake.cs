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
    SourceFilesExtensions.Add(".hlsl");
    SourceRootPath = ThisFileFolder;
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.LibraryFiles.Add("d3d12.lib");
    conf.LibraryFiles.Add("d3dcompiler.lib");
    conf.LibraryFiles.Add("dxgi.lib");
    conf.LibraryFiles.Add("dxguid.lib");

    conf.AddPublicDependency<IMGUI>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<DirectXExtra>(target);
    conf.AddPublicDependency<DirectXTex>(target);
    conf.AddPublicDependency<RexStd>(target);
  
    conf.AddPublicDependency<RexRendererCore>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexDXGI>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
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

  protected override void SetupPostBuildEvents(RexConfiguration conf, RexTarget target)
  {
    base.SetupPostBuildEvents(conf, target);

    switch (ProjectGen.Settings.GraphicsAPI)
    {
      case ProjectGen.GraphicsAPI.DirectX12:
        CompileShaders(conf, target);    
        break;
      default:
        break;
    }
  }

  private void CompileShaders(RexConfiguration conf, RexTarget target)
  {
    // Compile shaders here ...
    //
    // var dxc_path = "";
    // var out_dir = "";
    // var in_dir = "";
    //
    // conf.CustomBuildStep.Add(
    //             @"$(dxc_path) /T vs_5_1 /E main /Fo $(out_dir)\unlit_vs.cso $(in_dir)",
    //             new[] { dxc_path, out_dir, in_dir },
    //             new[] { @"$(out_dir)\\unlit_vs.cso" },
    //             new[] { in_dir }
    //         );
    //
    // conf.CustomBuildStep.Add(
    //             @"$(dxc_path) /T ps_5_1 /E main /Fo $(out_dir)\unlit_ps.cso $(in_dir)",
    //             new[] { dxc_path, out_dir, in_dir },
    //             new[] { @"$(out_dir)\\unlit_ps.cso" },
    //             new[] { in_dir }
    //         );
  }
}