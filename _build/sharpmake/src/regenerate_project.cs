using Sharpmake;
using System.IO;

// This is a very dirty hack but here's how it works
// Because Visual Studio will start the dependencies on the same node all at once
// We need to find a way to run a generation before any of the dependencies
// get run, but only do it once (not per project build)
// So what we do is, we create a dummy project that every project depends on
// which does nothing except rerunning sharpmake
// Because every project depends on this, this project will be put on the top node
// with no other projects at the same level, so this project gets "build" before any other
// and only once, resulting in the rerunning sharpmake only once

[Generate]
public class RegenerateProjects : Project
{
  public RegenerateProjects() : base(typeof(RexTarget), typeof(RexConfiguration))
  {
    // We need to mimic the targets generated, but only for visual studio IDE
    AddTargets(RexTarget.CreateTargetsForDevEnv(ProjectGen.Settings.IDE.ToDevEnv()).ToArray());
  }

  [Configure]
  public void Configure(RexConfiguration conf, RexTarget target)
  {
    // We need give the configuration a proper name or sharpmake fails to generate
    conf.Name = target.Config.ToString().ToLower();
    conf.ProjectPath = Path.Combine(Globals.BuildFolder, ProjectGen.Settings.IntermediateDir, target.DevEnv.ToString(), Name);
    conf.SolutionFolder = "_Generation";

    string rexpyPath = Path.Combine(Globals.Root, "_rex.py");

    // The custom build steps just perform a generation step
    string IdeCommandLineOption = "VisualStudio19";
    switch (ProjectGen.Settings.IDE)
    {
      case ProjectGen.IDE.VisualStudio: IdeCommandLineOption = "VisualStudio"; break;
      case ProjectGen.IDE.VisualStudio19: IdeCommandLineOption = "VisualStudio19"; break;
      case ProjectGen.IDE.VisualStudio22: IdeCommandLineOption = "VisualStudio22"; break;
      case ProjectGen.IDE.VSCode: IdeCommandLineOption = "VSCode"; break;
      default:
        break;
    }

    conf.CustomBuildSettings = new Configuration.NMakeBuildSettings();
    conf.CustomBuildSettings.BuildCommand = $"py {rexpyPath} generate -IDE None";
    conf.CustomBuildSettings.RebuildCommand = $"py {rexpyPath} generate -use-default-config -IDE {IdeCommandLineOption}"; // Perform a generation from scratch
    conf.CustomBuildSettings.CleanCommand = "";
    conf.CustomBuildSettings.OutputFile = "";
  }
}