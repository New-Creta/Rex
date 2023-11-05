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
      if (target.DevEnv == DevEnv.vs2019 && target.Compiler == Compiler.MSVC)
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
      DevEnv devEnv = DevEnv.ninja;
      switch (ProjectGen.Settings.IDE)
      {
        case ProjectGen.IDE.VisualStudio:
          devEnv |= DevEnv.vs2019;
          break;
        case ProjectGen.IDE.VSCode:
          devEnv |= DevEnv.vscode;
          break;
        default:
          break;
      }

      // The checks specified here are checks for various testing types
      // Thse checks do not work with Visual Studio and are only supported through the rex pipeline.
      if (ProjectGen.Settings.CoverageEnabled)
      {
        AddTargets(new RexTarget(Platform.win64, devEnv, Config.coverage, Compiler.Clang));
      }
      else if (ProjectGen.Settings.AsanEnabled)
      {
        AddTargets(new RexTarget(Platform.win64, devEnv, Config.address_sanitizer, Compiler.Clang));
      }
      else if (ProjectGen.Settings.UbsanEnabled)
      {
        AddTargets(new RexTarget(Platform.win64, devEnv, Config.undefined_behavior_sanitizer, Compiler.Clang));
      }
      else if (ProjectGen.Settings.FuzzyTestingEnabled)
      {
        AddTargets(new RexTarget(Platform.win64, devEnv, Config.fuzzy, Compiler.Clang));
      }
      else
      {
        // Always add the ninja target. Ninja is our main build system and is used what gets used by the rex development pipeline
        AddTargets(new RexTarget(Platform.win64, devEnv, Config.debug | Config.debug_opt | Config.release, Compiler.MSVC | Compiler.Clang));
      }
    }
  }
}