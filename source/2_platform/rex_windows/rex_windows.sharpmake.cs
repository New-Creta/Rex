using Sharpmake;
using System.IO;

[Generate]
public class RexWindows : PlatformProject
{
    public RexWindows() : base()
    {
        // The name of the project in Visual Studio. The default is the name of
        // the class, but you usually want to override that.
        Name = "RexWindows";
        GenerateTargets();

        string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
        SourceRootPath = ThisFileFolder;

        switch (ProjectGen.Settings.GraphicsAPI)
        {
            case ProjectGen.GraphicsAPI.OpenGL:
                SourceFilesBuildExcludeRegex.Add("win_directx_context.cpp");
                break;
            case ProjectGen.GraphicsAPI.DirectX12:
                SourceFilesBuildExcludeRegex.Add("win_opengl_context.cpp");
                break;
            default:
                break;
        }
    }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    switch (ProjectGen.Settings.GraphicsAPI)
    {
      case ProjectGen.GraphicsAPI.OpenGL:
        conf.AddPublicDependency<RexOpenGL>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
        break;
      case ProjectGen.GraphicsAPI.DirectX12:
        conf.AddPublicDependency<RexDirectX>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
        break;
      default:
        break;
    }
  }
}
