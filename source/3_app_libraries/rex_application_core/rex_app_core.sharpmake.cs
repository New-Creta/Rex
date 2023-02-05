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

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.Output = Configuration.OutputType.Lib;

    conf.add_dependency<RexWindows>(target);
  }
}
