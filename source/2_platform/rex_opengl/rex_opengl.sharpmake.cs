using Sharpmake;
using System.IO;

[Generate]
public class RexOpenGL : PlatformProject
{
  public RexOpenGL() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexOpenGL";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.LibraryFiles.Add("opengl32.lib");

    conf.AddPublicDependency<GLAD>(target);
    conf.AddPublicDependency<GLM>(target);
    conf.AddPublicDependency<RexRendererCore>(target);
    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
  }
}