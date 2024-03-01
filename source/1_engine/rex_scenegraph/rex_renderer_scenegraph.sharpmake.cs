using Sharpmake;
using System.IO;

[Generate]
public class RexScenegraph : EngineProject
{
  public RexScenegraph() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexScenegraph";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexStd>(target);
    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexRendererResources>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
  }
}