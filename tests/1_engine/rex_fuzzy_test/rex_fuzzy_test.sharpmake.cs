using Sharpmake;
using System.IO;

[Generate]
public class RexFuzzyTest : TestProject
{
  public RexFuzzyTest() : base()
  {
    Name = GenerateName("RexFuzzyTest");
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    ProjectType = ProjectGen.TestProjectType.Undefined;
  }

  public override void GenerateTargets()
  {
    AddTargets(new RexTarget(Platform.win64, DevEnv.ninja, Config.fuzzy, Compiler.Clang));

    if (ProjectGen.Settings.IDE == ProjectGen.IDE.VisualStudio && Util.GetVisualStudioInstallationsFromQuery(DevEnv.vs2019).Count > 0)
    {
      AddTargets(new RexTarget(Platform.win64, DevEnv.vs2019, Config.fuzzy, Compiler.Clang));
    }
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    base.SetupSolutionFolder(conf, target);

    conf.SolutionFolder = Path.Combine(conf.SolutionFolder, "1_engine");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.Options.Remove(Options.Vc.Compiler.JumboBuild.Enable);
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Lib;
  }
}
