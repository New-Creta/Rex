using System.IO;
using System.Linq;
using Sharpmake;
using System;
using System.Collections.Generic;

namespace rex
{
  // Represents the solution that will be generated and that will contain the
  // project with the sample code.
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

      if (GenerateSettings.UnitTestsEnabled)
      {
        conf.AddProject<RexStdTest>(target);
      }

      if (GenerateSettings.FuzzyTestingEnabled)
      {
        conf.AddProject<RexStdFuzzy>(target);
      }

      if (GenerateSettings.AutoTestsEnabled)
      {
        conf.AddProject<ReginaTest>(target);
      }

      else if (GenerateSettings.EnableDefaultGeneration)
      {
        conf.AddProject<Regina>(target);
        //conf.AddProject<ConsoleApp>(target);
      }

    }

    protected string GenerateName(string baseName)
    {
      return baseName;
    }

    protected void GenerateTargets()
    {
      if (GenerateSettings.IDE == GenerationTypes.IDE.VisualStudio)
      {
        AddTargets(RexTarget.GetAllDefaultTargets());
      }
      else if (GenerateSettings.CoverageEnabled)
      {
        AddTargets(RexTarget.GetCoverageTarget());
      }
      else if (GenerateSettings.AsanEnabled)
      {
        AddTargets(RexTarget.GetAsanTarget());
      }
      else if (GenerateSettings.UbsanEnabled)
      {
        AddTargets(RexTarget.GetUBsanTarget());
      }
      else if (GenerateSettings.FuzzyTestingEnabled)
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