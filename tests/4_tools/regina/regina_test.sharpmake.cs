using Sharpmake;
using System.IO;

[Generate]
public class ReginaTest : TestProject
{
  public ReginaTest() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "ReginaTest";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    string relative_source_path = Util.PathGetRelative(Path.Combine(Globals.SourceRoot), SourceRootPath);
    GenerationConfigPath = Path.Combine(Globals.Root, "config", relative_source_path, "generation.json");
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.AddPublicDependency<Regina>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);

    conf.Output = Configuration.OutputType.Exe;

  }
}