using Sharpmake;
using System.IO;

[Generate]
public class RexEngineUnitTest : TestProject
{
  public RexEngineUnitTest() : base()
  {
    Name = GenerateName("RexEngineUnitTest");
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    ProjectType = ProjectGen.TestProjectType.UnitTest;
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
    conf.AddPublicDependency<RexUnitTest>(target, DependencySetting.Default);
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.Options.Remove(Options.Vc.Compiler.JumboBuild.Enable);

    if (target.Compiler == Compiler.MSVC)
    {
      conf.AdditionalCompilerOptions.Add("/utf-8");
    }
  }
}
