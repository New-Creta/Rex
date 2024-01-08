using Sharpmake;

namespace rex
{
  // Represents the solution that will be generated
  [Generate]
  public class MainSolution : Solution
  {
    public MainSolution() : base(typeof(RexTarget))
    {
      // The name of the solution.
      Name = GenerateName("rex");
      GenerateTargets();
    }

    // Configure for all 4 generated targets. Note that the type of the
    // configuration object is of type Solution.Configuration this time.
    // (Instead of Project.Configuration.)
    [Configure]
    public void Configure(Configuration conf, RexTarget target)
    {
      // Puts the generated solution in the root folder.
      conf.SolutionPath = Globals.Root;

      // Because the sharpmake project only gets added to Visual Studio
      // We can only add its dependency if the target development env is Visual Studio
      if (target.DevEnv == DevEnv.vs2019 && target.Compiler == Compiler.MSVC)
      {
        conf.AddProject<SharpmakeProject>(target);
      }

      if (ProjectGen.Settings.UnitTestsEnabled)
      {
        conf.AddProject<RexStdTest>(target);
        conf.AddProject<RexEngineUnitTest>(target);
      }

      if (target.Config == Config.fuzzy)
      {
        conf.AddProject<RexStdFuzzy>(target);
        conf.AddProject<RexEngineFuzzyTest>(target);
      }

      if (ProjectGen.Settings.AutoTestsEnabled)
      {
        conf.AddProject<ReginaAutoTest>(target);
      }

      conf.AddProject<Regina>(target);
    }

    protected string GenerateName(string baseName)
    {
      return baseName;
    }

    protected void GenerateTargets()
    {
      AddTargets(RexTarget.CreateTargets().ToArray());
    }
  }
}