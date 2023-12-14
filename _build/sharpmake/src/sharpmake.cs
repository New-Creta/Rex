using System.IO;
using System.Linq;
using Sharpmake;
using System;
using System.Collections.Generic;

namespace rex
{
  // The sharpmake project that generates the solution
  // It makes life a lot easier if this is directly in the solution.
  [Generate]
  public class SharpmakeProject : BasicCSProject
  {
    public SharpmakeProject() : base()
    {
      Name = "SharpmakeProject";
      SourceFilesExtensions.Clear();
      SourceFilesExtensions.Add(".sharpmake.cs");
      SourceRootPath = Path.Combine(Globals.Root, "source");
      AdditionalSourceRootPaths.Add(Path.Combine(Globals.Root, "tests"));

      // manually add the sharpmake root files
      var RootSharpmakeFiles = Directory.GetFiles(Path.Combine(Globals.SharpmakeRoot, "src"));
      foreach (var File in RootSharpmakeFiles)
      {
        SourceFiles.Add(File);
      }

      RexTarget vsTarget = new RexTarget(Platform.win64, DevEnv.vs2019, Config.debug | Config.debug_opt | Config.release, Compiler.MSVC);

      // Specify the targets for which we want to generate a configuration for.
      AddTargets(vsTarget);
    }

    protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
    {
      conf.Output = Configuration.OutputType.DotNetClassLibrary;
    }

    // Setup default configuration settings for C++ projects
    protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
    {
      base.SetupConfigSettings(conf, target);

      conf.StartWorkingDirectory = Globals.SharpmakeRoot;
    }

    protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
    {
      base.SetupLibDependencies(conf, target);

      string sharpmakeAppPath = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
      string sharpmakeDllPath = Path.Combine(Path.GetDirectoryName(sharpmakeAppPath), "sharpmake.dll");

      conf.ReferencesByPath.Add(sharpmakeDllPath);
      conf.ReferencesByName.AddRange(new Strings("System",
                                                 "System.Core",
                                                 "System.Memory"
                                                 ));

      conf.ReferencesByNuGetPackage.Add("System.Text.Json", "5.0.2"); // same version sharpmake uses

      List<string> listOfArgs = Environment.GetCommandLineArgs().ToList();
      listOfArgs.RemoveAt(0); // Removing the first argument as it's the path to sharpmake
      string argsAsString = string.Join(" ", listOfArgs);

      // We need to quote all our sources
      // we extract our sources string, and quote all of them
      string sourcesPrefix = "/sources(";
      int sourcesPrefixStart = argsAsString.IndexOf(sourcesPrefix);
      int closeBracketPos = argsAsString.IndexOf(')', sourcesPrefixStart);
      string sources = argsAsString.Substring(sourcesPrefixStart + sourcesPrefix.Length, closeBracketPos - (sourcesPrefixStart + sourcesPrefix.Length));
      
      string args = "";
      args += argsAsString.Substring(0, sourcesPrefixStart + sourcesPrefix.Length);
      args += "\"";
      args += sources.Replace(", ", "\", \"");
      args += "\"";
      args += argsAsString.Substring(closeBracketPos);

      conf.CsprojUserFile = new Configuration.CsprojUserFileSettings();
      conf.CsprojUserFile.StartAction = Configuration.CsprojUserFileSettings.StartActionSetting.Program;
      conf.CsprojUserFile.StartArguments = $@"{args}";
      conf.CsprojUserFile.StartProgram = sharpmakeAppPath;
      conf.CsprojUserFile.WorkingDirectory = Directory.GetCurrentDirectory();
    }
  }
}