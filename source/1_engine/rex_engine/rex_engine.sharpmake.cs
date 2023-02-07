using Sharpmake;
using System.IO;
using System.Text.Json;
using System.Collections.Generic;
using System.Linq;

[Generate]
public class RexEngine : EngineProject
{
  public RexEngine() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexEngine";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.Output = Configuration.OutputType.Lib;

    conf.AddPublicDependency<RexStdExtra>(target, DependencySetting.Default);

    switch (target.Config)
    {
      case Config.debug:
      case Config.debug_opt:
      case Config.address_sanitizer:
      case Config.undefined_behavior_sanitizer:
      case Config.fuzzy:
        conf.add_public_define("REX_DEBUG");
        break;
    }
  }
}