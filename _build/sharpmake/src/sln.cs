using System.IO;
using System.Linq;
using Sharpmake;
using System;
using System.Collections.Generic;

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
      if (target.DevEnv == DevEnv.vs2019)
      {
        conf.AddProject<SharpmakeProject>(target);
      }

      if (ProjectGen.Settings.UnitTestsEnabled)
      {
        conf.AddProject<RexStdTest>(target);
      }

      if (ProjectGen.Settings.FuzzyTestingEnabled)
      {
        conf.AddProject<RexStdFuzzy>(target);
      }

      if (ProjectGen.Settings.AutoTestsEnabled)
      {
        conf.AddProject<ReginaTest>(target);
      }

      conf.AddProject<Regina>(target);
      //conf.AddProject<ConsoleApp>(target);

    }

    protected string GenerateName(string baseName)
    {
      return baseName;
    }

    protected void GenerateTargets()
    {
      if (ProjectGen.Settings.IDE == ProjectGen.IDE.VisualStudio)
      {
        AddTargets(RexTarget.GetAllDefaultTargets());
      }
      else if (ProjectGen.Settings.CoverageEnabled)
      {
        AddTargets(RexTarget.GetCoverageTarget());
      }
      else if (ProjectGen.Settings.AsanEnabled)
      {
        AddTargets(RexTarget.GetAsanTarget());
      }
      else if (ProjectGen.Settings.UbsanEnabled)
      {
        AddTargets(RexTarget.GetUBsanTarget());
      }
      else if (ProjectGen.Settings.FuzzyTestingEnabled)
      {
        AddTargets(RexTarget.GetFuzzyTarget());
      }
      else
      {
        AddTargets(RexTarget.GetNinjaOnlyTarget());
      }
    }
  }
}