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

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.Output = Configuration.OutputType.Lib;

    conf.AddPublicDependency<GLEW>(target);
    conf.AddPublicDependency<GLM>(target);
    conf.AddPublicDependency<RexStdExtra>(target);
    conf.AddPublicDependency<RexRendererCore>(target);
    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
  }
}