using System.IO;
using System.Linq;
using Sharpmake;
using System;
using System.Collections.Generic;
using System.Text.Json;
using System.Text;

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
    conf.ProjectPath = Path.Combine(Globals.BuildFolder, target.DevEnv.ToString(), Name);
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

    if (GenerateSettings.EnableAutoTests)
    {
      conf.AddProject<ReginaTest>(target);
    }

    else if (GenerateSettings.EnableDefaultGeneration)
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

    [Sharpmake.CommandLine.Option("enableSecondPass")]
    public void CommandLinePerformAllChecks()
    {
      GenerateSettings.EnableSecondPass = true;
    }

    [Sharpmake.CommandLine.Option("intermediateDir")]
    public void CommandLineIntermediateDir(string intermediateDir)
    {
      GenerateSettings.IntermediateDir = intermediateDir;
    }

    [Sharpmake.CommandLine.Option("generateUnitTests")]
    public void CommandLineGenerateUnitTests()
    {
      GenerateSettings.GenerateUnitTests = true;
    }

    [Sharpmake.CommandLine.Option("enableCodeCoverage")]
    public void CommandLineEnableCodeCoverage()
    {
      GenerateSettings.CoverageEnabled = true;
    }

    [Sharpmake.CommandLine.Option("enableAsan")]
    public void CommandLineEnableAsan()
    {
      GenerateSettings.AddressSanitizerEnabled = true;
    }

    [Sharpmake.CommandLine.Option("enableUBsan")]
    public void CommandLineEnableUBsan()
    {
      GenerateSettings.UndefinedBehaviorSanitizerEnabled = true;
    }

    [Sharpmake.CommandLine.Option("enableFuzzyTests")]
    public void CommandLineEnableFuzzyTests()
    {
      GenerateSettings.GenerateFuzzyTests = true;
    }

    [Sharpmake.CommandLine.Option("enableVisualStudio")]
    public void CommandLineEnableVisualStudio()
    {
      GenerateSettings.EnableVisualStudio = true;
    }

    [Sharpmake.CommandLine.Option("noClangTools")]
    public void CommandLineDisableClangTools()
    {
      GenerateSettings.NoClangTools = true;
    }

    [Sharpmake.CommandLine.Option("disableDefaultGeneration")]
    public void CommandLineDisableDefaultGeneration()
    {
      GenerateSettings.EnableDefaultGeneration = false;
    }

    [Sharpmake.CommandLine.Option("GraphicsAPI")]
    public void CommandLineGraphicsAPI(string graphicsAPI)
    {
        string lower_graphics_api = graphicsAPI.ToLower();
        if(lower_graphics_api == "directx12")
            {
                GenerateSettings.GraphicsAPI = GraphicsAPI.DirectX12;
            }
        else if(lower_graphics_api == "opengl")
            {
                GenerateSettings.GraphicsAPI = GraphicsAPI.OpenGL;
            }
        else
            {
                Console.WriteLine("[WARNING] Invalid graphics API (directx12 | opengl), reverting to default for this platform.");
            }
    }

    [Sharpmake.CommandLine.Option("disableClangTidyForThirdParty")]
    public void CommandLineDisableClangTidyForThirdParty()
    {
      GenerateSettings.DisableClangTidyForThirdParty = true;
    }

    [Sharpmake.CommandLine.Option("enableAutoTests")]
    public void CommandLineEnableAutoTests()
    {
      GenerateSettings.EnableAutoTests = true;
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

    InitializeGenerationSettings();
    InitializeSharpmake();

    arguments.Generate<MainSolution>();

    Builder.Instance.EventPostGeneration += PostGenerationEvent;
  }

  private static void PostGenerationEvent(List<Project> projects, List<Solution> solutions)
  {
    GenerateCompilerDatabases();
    AutoGenerateEnums();
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

  private static void AutoGenerateEnums()
  {
    foreach (EnumGenerationSettings enum_gen_settings in GenerateSettings.EnumsToAutoGenerate.Values)
    {
      WriteAutoGeneratedEnumToFile(enum_gen_settings.ClassName, enum_gen_settings.ProjectToEnumValues, enum_gen_settings.Filepath, enum_gen_settings.ProjectToGenerationFile);
    }

    foreach (ArrayGenerationSettings array_gen_settings in GenerateSettings.ArraysToAutoGenerate.Values)
    {
      WriteAutoGeneratedArrayToFile(array_gen_settings.ElementType, array_gen_settings.Name, array_gen_settings.Includes, array_gen_settings.ProjectToArrayValues, array_gen_settings.Filepath, array_gen_settings.ProjectToGenerationFile);
    }
  }

  private static void WriteAutoGeneratedEnumToFile(string className, Dictionary<string, List<string>> enumValues, string filename, Dictionary<string, string> projectToGenerationFile)
  {
    StringBuilder sb = new StringBuilder();
    WriteCustomGenerationHeader(sb);
    WriteBeginNamespace(sb);

    sb.AppendLine($"  enum class {className}");
    sb.AppendLine("  {");

    foreach (var project_vals in enumValues)
    {
      sb.AppendLine($"    // {className} values for {project_vals.Key}");
      sb.AppendLine($"    // generated from {projectToGenerationFile[project_vals.Key]}");

      foreach (string val in project_vals.Value)
      {
        sb.AppendLine($"    {val},");
      }
      sb.AppendLine(" ");
    }

    sb.AppendLine("  };");

    WriteEndNamespace(sb);
    WriteCustomGenerationFooter(sb);
    WriteToDisk(sb, Path.Combine(Globals.SourceRoot, filename));
  }

  private static void WriteAutoGeneratedArrayToFile(string elementType, string name, List<string> includes, Dictionary<string, List<string>> arrayValues, string filename, Dictionary<string, string> projectToGenrationFile)
  {
    StringBuilder sb = new StringBuilder();

    WriteCustomGenerationHeader(sb);

    foreach (string include in includes)
    {
      sb.AppendLine($"#include \"{include}\"");
    }

    sb.AppendLine($"#include \"rex_std/array.h\"");

    WriteBeginNamespace(sb);

    sb.AppendLine($"  inline rsl::array {name} = ");
    sb.AppendLine("  {");

    foreach (var project_vals in arrayValues)
    {
      sb.AppendLine($"    // {elementType} values for {project_vals.Key}");
      sb.AppendLine($"    // generated from {projectToGenrationFile[project_vals.Key]}");

      foreach (string val in project_vals.Value)
      {
        sb.AppendLine($"    {elementType}{{ {val}, \"{project_vals.Key}\" }},");
      }
      sb.AppendLine(" ");
    }

    sb.AppendLine("  };");

    WriteEndNamespace(sb);
    WriteCustomGenerationFooter(sb);
    WriteToDisk(sb, Path.Combine(Globals.SourceRoot, filename));
  }

  private static void WriteCustomGenerationHeader(StringBuilder sb)
  {
    sb.AppendLine("#pragma once");
    sb.AppendLine("");
    sb.AppendLine("// DON'T EDIT - This file is auto generated by sharpmake");
    sb.AppendLine("// NOLINTBEGIN");
    sb.AppendLine("");
  }

  private static void WriteBeginNamespace(StringBuilder sb, string namespaceName = "rex")
  {
    sb.AppendLine($"namespace {namespaceName}");
    sb.AppendLine($"{{");
  }

  private static void WriteCustomGenerationFooter(StringBuilder sb)
  { }

  private static void WriteEndNamespace(StringBuilder sb, string namespaceName = "rex")
  {
    sb.AppendLine($"}} // namespace {namespaceName}");
    sb.AppendLine($"// NOLINTEND");
  }

  private static void WriteToDisk(StringBuilder sb, string filePath)
  {
    FileStream stream = File.Open(filePath, FileMode.Truncate);

    byte[] bytes = Encoding.ASCII.GetBytes(sb.ToString());
    stream.Write(bytes, 0, sb.Length);
    stream.Close();

  }

  private static void InitializeSharpmake()
  {
    InitializeNinja();

    // Initialize Visual Studio settings
    KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2019, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.v10_0_19041_0);
  }

    private static void InitializeGraphicsAPI()
    {
        if (GenerateSettings.GraphicsAPI == GraphicsAPI.Unknown)
        {
            OperatingSystem os = Environment.OSVersion;
            switch (os.Platform)
            {
                case PlatformID.Win32NT:
                case PlatformID.Xbox:
                    GenerateSettings.GraphicsAPI = GraphicsAPI.DirectX12;
                    break;
                case PlatformID.Unix:
                case PlatformID.MacOSX:
                    GenerateSettings.GraphicsAPI = GraphicsAPI.OpenGL;
                    break;
                default:
                    Console.WriteLine("[WARNING] Could not determine OS, reverting graphics API to OpenGL");
                    GenerateSettings.GraphicsAPI = GraphicsAPI.OpenGL;
                    break;
            }
        }

        Console.WriteLine($"Using Graphics API: {GenerateSettings.GraphicsAPI}");
    }

  private static void InitializeGenerationSettings()
  {
        InitializeGraphicsAPI();
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