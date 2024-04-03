using Sharpmake;
using System.IO;

// The project representing the project you'll be using for testing
[Generate]
public class <AutoTestProjectName> : TestProject
{
  // The constructor does some basic initialization, filling in date for the base class
  public <AutoTestProjectName>() : base()
  {
    // The name of this project
    Name = GenerateName("<AutoTestProjectName>");
    // Generate the targets for this project. Making sure you can build it in the desired configs
    GenerateTargets();

    // Indicate where our source root starts.
    // Sharpmake will recursively go through path and add any file it finds (with the correct extension)
    // to our project.
    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    // Specify this is a auto test project
    ProjectType = ProjectGen.TestProjectType.AutoTest;
  }

  // Specify the solution folder.
  // It's recommended to make the solution folder that's "test_solution_folder"
  // match the solution folder of the project it's testing. (Eg. 4_tools)
  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    base.SetupSolutionFolder(conf, target);

    conf.SolutionFolder = Path.Combine(conf.SolutionFolder, "test_solution_folder");
  }

  // Set up the library dependencies of this project.
  // You should link to RexAutoTest by default, as this is a auto test project
  // Secondly, you should link to the project you're actually writing tests for.
  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    conf.AddPublicDependency<RexAutoTest>(target, DependencySetting.Default);
  }

  // By default, disable unity builds as they serve little purpose in unit test.
  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.Options.Remove(Options.Vc.Compiler.JumboBuild.Enable);
  }
}