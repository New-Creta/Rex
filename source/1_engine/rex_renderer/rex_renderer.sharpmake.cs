using Sharpmake;
using System.IO;

[Generate]
public class RexRenderer : EngineProject
{
  public RexRenderer() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexRenderer";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.Output = Configuration.OutputType.Lib;

    conf.add_dependency<RexStdExtra>(target);
    conf.add_dependency<RexEngine>(target);
  }
}