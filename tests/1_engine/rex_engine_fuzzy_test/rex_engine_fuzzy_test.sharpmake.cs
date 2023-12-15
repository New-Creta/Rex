using Sharpmake;
using System.IO;

[Generate]
public class RexEngineFuzzyTest : TestProject
{
  public RexEngineFuzzyTest() : base()
  {
    Name = GenerateName("RexEngineFuzzyTest");
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    ProjectType = ProjectGen.TestProjectType.Fuzzy;
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
    conf.AddPublicDependency<RexFuzzyTest>(target, DependencySetting.Default);
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.Options.Remove(Options.Vc.Compiler.JumboBuild.Enable);
  }
}
