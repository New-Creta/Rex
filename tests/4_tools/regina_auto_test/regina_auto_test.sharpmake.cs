using Sharpmake;
using System.IO;

[Generate]
public class ReginaAutoTest : TestProject
{
  public ReginaAutoTest() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "ReginaAutoTest";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    ProjectType = ProjectGen.TestProjectType.AutoTest;
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    base.SetupSolutionFolder(conf, target);

    conf.SolutionFolder = Path.Combine(conf.SolutionFolder, "4_tools");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<Regina>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
  }
}
