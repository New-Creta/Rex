using Sharpmake;
using System.IO;

[Generate]
public class RexEngineAutoTest : TestProject
{
  public RexEngineAutoTest() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexEngineAutoTest";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    ProjectType = ProjectGen.TestProjectType.AutoTest;
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    base.SetupSolutionFolder(conf, target);

    conf.SolutionFolder = Path.Combine(conf.SolutionFolder, "1_engine");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexAutoTest>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexWindows>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools); // We depent on Windows so we can create a console application to write our auto tests in
  }
}
