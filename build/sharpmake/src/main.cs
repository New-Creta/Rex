using System.IO;
using System.Linq;
using Sharpmake;
using System;
using System.Collections.Generic;
using System.Text.Json;

[module: Sharpmake.Reference("System.Text.Json.dll")]
[module: Sharpmake.Reference("System.Memory.dll")]

// The sharpmake project that generates the solution
// It makes life a lot easier if this is directly in the solution.
[Generate]
public class SharpmakeProject : CSharpProject
{
  public SharpmakeProject() : base(typeof(RexTarget), typeof(RexConfiguration))
  {
    SourceFilesExtensions.Clear();
    SourceFilesExtensions.Add(".sharpmake.cs");
    SourceRootPath = Globals.Root;

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

  [Configure()]
  public virtual void Configure(RexConfiguration conf, RexTarget target)
  {
    conf.ProjectPath = Path.Combine(Globals.Root, ".rex", "build", target.DevEnv.ToString(), Name);
    conf.IntermediatePath = Path.Combine(conf.ProjectPath, "intermediate", conf.Name, target.Compiler.ToString());
    conf.TargetPath = Path.Combine(conf.ProjectPath, "bin", conf.Name);
    conf.Output = Configuration.OutputType.DotNetClassLibrary;
    conf.StartWorkingDirectory = Globals.SharpmakeRoot;

    string sharpmakeAppPath = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
    string sharpmakeDllPath = Path.Combine(Path.GetDirectoryName(sharpmakeAppPath), "sharpmake.dll");

    conf.ReferencesByPath.Add(sharpmakeDllPath);
    conf.ReferencesByName.AddRange(new Strings("System",
                                               "System.Core",
                                               "System.Memory"
                                               ));

    conf.ReferencesByNuGetPackage.Add("System.Text.Json", "5.0.2"); // same version sharpmake uses

    conf.CsprojUserFile = new Configuration.CsprojUserFileSettings();
    conf.CsprojUserFile.StartAction = Configuration.CsprojUserFileSettings.StartActionSetting.Program;

    string quote = "\'"; // Use single quote that is cross platform safe
    List<string> sharpmake_sources = new List<string>();
    foreach (string sourceFile in SourceFiles)
    {
      string file = sourceFile.Replace('\\', '/');
      sharpmake_sources.Add($"{quote}{file}{quote}");
    }

    foreach (string file in Directory.EnumerateFiles(SourceRootPath, "*.*", SearchOption.AllDirectories))
    {
      if (file.EndsWith(".sharpmake.cs"))
      {
        sharpmake_sources.Add($"{quote}{file}{quote}");
      }
    }

    string sourcesArg = @"/sources(";
    foreach (string file in sharpmake_sources)
    {
      sourcesArg += file;
      sourcesArg += ", ";
    }
    sourcesArg = sourcesArg.Substring(0, sourcesArg.Length - 2); // remove ", ";
    sourcesArg += ")";

    conf.CsprojUserFile.StartArguments = $@"{sourcesArg} /diagnostics";
    conf.CsprojUserFile.StartProgram = sharpmakeAppPath;
    conf.CsprojUserFile.WorkingDirectory = Directory.GetCurrentDirectory();
  }
}

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

    if (target.DevEnv == DevEnv.vs2019)
    {
      conf.AddProject<SharpmakeProject>(target);
    }

    if (GenerateSettings.GenerateUnitTests)
    {
      conf.AddProject<RexStdTest>(target);
    }

    if (GenerateSettings.GenerateFuzzyTests)
    {
      conf.AddProject<RexStdFuzzy>(target);
    }

    if (GenerateSettings.EnableDefaultGeneration)
    {
      conf.AddProject<Regina>(target);
    }
  }

  protected string GenerateName(string baseName)
  {
    return baseName;
  }

  protected void GenerateTargets()
  {
    if (GenerateSettings.EnableVisualStudio)
    {
      AddTargets(RexTarget.GetAllDefaultTargets());
    }
    else if (GenerateSettings.CoverageEnabled)
    {
      AddTargets(RexTarget.GetCoverageTarget());
    }
    else if (GenerateSettings.AddressSanitizerEnabled)
    {
      AddTargets(RexTarget.GetAsanTarget());
    }
    else if (GenerateSettings.UndefinedBehaviorSanitizerEnabled)
    {
      AddTargets(RexTarget.GetUBsanTarget());
    }
    else if (GenerateSettings.GenerateFuzzyTests)
    {
      AddTargets(RexTarget.GetFuzzyTarget());
    }
    else
    {
      AddTargets(RexTarget.GetNinjaOnlyTarget());
    }
  }
}

namespace rex
{
  public class CmdLineArguments
  {
    [Sharpmake.CommandLine.Option("clangTidyRegex", "Add this regex to clang-tidy to filter which files it should process")]
    public void CommandLineClangTiyRegex(string clangTidyRegex)
    {
      GenerateSettings.ClangTidyRegex = clangTidyRegex;
    }

    [Sharpmake.CommandLine.Option("performAllChecks")]
    public void CommandLinePerformAllChecks(bool performAllChecks)
    {
      GenerateSettings.PerformAllChecks = performAllChecks;
    }

    [Sharpmake.CommandLine.Option("intermediateDir")]
    public void CommandLineIntermediateDir(string intermediateDir)
    {
      GenerateSettings.IntermediateDir = intermediateDir;
    }

    [Sharpmake.CommandLine.Option("GenerateUnitTests")]
    public void CommandLineGenerateUnitTests()
    {
      GenerateSettings.GenerateUnitTests = true;
    }

    [Sharpmake.CommandLine.Option("EnableCodeCoverage")]
    public void CommandLinePerformAllChecks()
    {
      GenerateSettings.CoverageEnabled = true;
    }

    [Sharpmake.CommandLine.Option("EnableAsan")]
    public void CommandLineEnableAsan()
    {
      GenerateSettings.AddressSanitizerEnabled = true;
    }

    [Sharpmake.CommandLine.Option("EnableUBsan")]
    public void CommandLineEnableUBsan()
    {
      GenerateSettings.UndefinedBehaviorSanitizerEnabled = true;
    }

    [Sharpmake.CommandLine.Option("EnableFuzzyTests")]
    public void CommandLineEnableFuzzyTests()
    {
      GenerateSettings.GenerateFuzzyTests = true;
    }

    [Sharpmake.CommandLine.Option("EnableVisualStudio")]
    public void CommandLineEnableVisualStudio()
    {
      GenerateSettings.EnableVisualStudio = true;
    }

    [Sharpmake.CommandLine.Option("NoClangTools")]
    public void CommandLineDisableClangTools()
    {
      GenerateSettings.NoClangTools = true;
    }

    [Sharpmake.CommandLine.Option("DisableDefaultGeneration")]
    public void CommandLineDisableDefaultGeneration()
    {
      GenerateSettings.EnableDefaultGeneration = false;
    }

  }
}

public static class Main
{
  [Sharpmake.Main]
  public static void SharpmakeMain(Arguments arguments)
  {
    rex.CmdLineArguments Arguments = new rex.CmdLineArguments();
    CommandLine.ExecuteOnObject(Arguments);

    Globals.Init();

    InitializeSharpmake();

    arguments.Generate<MainSolution>();

    Builder.Instance.EventPostGeneration += PostGenerationEvent;
  }

  private static void PostGenerationEvent(List<Project> projects, List<Solution> solutions)
  {
    GenerateCompilerDatabases();
  }

  private static void GenerateCompilerDatabases()
  {
    Console.WriteLine("Generating compiler databases");
    foreach (System.Diagnostics.Process proc in GenerateSettings.GenerateCompilerDBProcesses)
    {
      proc.Start();
      proc.WaitForExit();
    }
  }

  private static void InitializeSharpmake()
  {
    InitializeNinja();

    // Initialize Visual Studio settings
    KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2019, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.v10_0_19041_0);
  }

  private static void InitializeNinja()
  {
    string tools_json_path = Path.Combine(Globals.ToolsRoot, "tool_paths.json");
    string json_blob = File.ReadAllText(tools_json_path);
    Dictionary<string, string> paths = JsonSerializer.Deserialize<Dictionary<string, string>>(json_blob);

    KitsRootPaths.SetCompilerPaths(Compiler.MSVC, paths["msvc_compiler_path"], paths["msvc_linker_path"], paths["msvc_archiver_path"], "");
    KitsRootPaths.SetCompilerPaths(Compiler.Clang, paths["clang_compiler_path"], paths["clang_linker_path"], paths["clang_archiver_path"], paths["clang_ranlib_path"]);
    KitsRootPaths.SetNinjaPath(paths["ninja_path"]);
  }
}