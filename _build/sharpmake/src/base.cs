using System.IO;
using System.Linq;
using Sharpmake;
using System.Collections.Generic;
using System.Text.Json;

// This file defines the base class for all different kind of projects supported for rex.
// The BaseProject is cross-language and defines things like config name, intermediate directory, ...
// There are also base classes for each project based on language (eg. BasicCPPProject).
// Lastly there are project type base classes (eg. EngineProject, ThirdPartyProject, ..)
// Each project added to the rex solution is meant to inherit from project type classes.

// Because C# doesn't allow inheritance by generic, we need to find a different
// of sharing logic with C++ and C# projects.
// I've come up with the following idea..
// The below class defines generic logic to be shared between C++ and C# projects
// BasicCPPProject and BasicCSProject call into the below class' static functions
// for generic setup, derived classes further down the line don't care about this
// It's not the most ideal way of doing it, but it works..
public class BaseConfiguration
{
  private Project Project;

  public BaseConfiguration(Project project)
  {
    Project = project;
  }

  // This is called by configure functions of top level project types
  public void Configure(RexConfiguration conf, RexTarget target)
  {
    conf.Name = string.Concat(target.Config.ToString().ToLower(), target.Compiler.ToString().ToLower());
    conf.DumpDependencyGraph = true;

    // These are private and are not virtualized to be configurable derived projects
    SetupProjectPaths(conf, target);
    SetupDefaultConfigurationSettings(conf, target);

    // These are protected and optionally changed by derived projects
    SetupSolutionFolder(conf, target);
  }

  // Setup project paths like the project path itself, intermediate path, target path, pdb paths, ..
  private void SetupProjectPaths(RexConfiguration conf, RexTarget target)
  {
    conf.ProjectPath = Path.Combine(Globals.BuildFolder, ProjectGen.Settings.IntermediateDir, target.DevEnv.ToString(), Project.Name);
    conf.TargetFileName = $"{conf.TargetFileName}_{target.ProjectConfigurationName}_{target.Compiler}";
  }
  // Setup default configuration settings.
  private void SetupDefaultConfigurationSettings(RexConfiguration conf, RexTarget target)
  {
    // General options
    conf.Options.Add(Options.Vc.General.CharacterSet.MultiByte);
    conf.Options.Add(Options.Vc.General.PlatformToolset.v142);
    conf.Options.Add(Options.Vc.General.WarningLevel.Level4);

    // Disable warning as errors in debug so that we can add debug code
    // without the compiler warning about it.
    // We just need to make sure we delete it before it goes in to version control
    if (target.Config != Config.debug)
    {
      conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Enable);
    }
  }
  // Setup the solution folder of this project.
  private void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    // Setup solution folder..
    // Ideally we want to specify the following in the third party project
    // But Rex Std is a BasicCPPProject, so we have to hack around it like this
    // Rex Std is a BasicCPPProject because it is a standalone repository
    // In which it's not a thirdparty project
    // Because Rex Std is its own repository, also using sharpmake and in it
    // The project definition comes from the repository there.
    // Base config doesn't setup the solution folder, other than the thirdparty folder
    if (Utils.FindInParent(Project.SourceRootPath, "0_thirdparty") != "")
    {
      conf.SolutionFolder = "0_thirdparty";
    }
  }
}

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
    AddTargets(RexTarget.CreateTargetsForDevEnv(DevEnv.vs2019).ToArray());
  }

  [Configure]
  public void Configure(RexConfiguration conf, RexTarget target)
  {
    // We need give the configuration a proper name or sharpmake fails to generate
    conf.Name = string.Concat(target.Config.ToString().ToLower(), target.Compiler.ToString().ToLower());
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
    conf.CustomBuildSettings.BuildCommand = $"py {rexpyPath} generate -IDE {IdeCommandLineOption}"; // Use what's previously generated
    conf.CustomBuildSettings.RebuildCommand = $"py {rexpyPath} generate -use_default_config -IDE {IdeCommandLineOption}"; // Perform a generation from scratch
    conf.CustomBuildSettings.CleanCommand = "";
    conf.CustomBuildSettings.OutputFile = "";
  }
}

// This is the base class for every C++ project used in the rex solution
// Some of its functionality is sharedwith BasicCSProject through BaseConfiguration
public abstract class BasicCPPProject : Project
{
  // holds the paths to tools needed to generate/build/run/test rex engine
  private Dictionary<string, string[]> ToolPaths;
  // lock to add to the generate compiler db queue. Sharpmake runs multithreaded so we need to lock when adding to the list
  static private object LockToCompilerDBGenerationQueue = new object();

  // indicates if the project creates a compiler DB for itself
  protected bool ClangToolsEnabled = true;

  public BasicCPPProject() : base(typeof(RexTarget), typeof(RexConfiguration))
  {
    LoadToolPaths();

    ClangToolsEnabled = ProjectGen.Settings.ClangToolsEnabled;

    ResourceFilesExtensions.Add(".json");
  }

  // Legacy function and should be removed
  public string GenerateName(string name)
  {
    return name;
  }

  // This gets called before any configuration gets performed
  public override void PreConfigure()
  {
    base.PreConfigure();

    // This is a dirty hack for rex std which is technically a thirdparty project
    // but inherits from BasicCPPProject.
    if (ProjectGen.Settings.DisableClangTidyForThirdParty && SourceRootPath.Contains(Globals.ThirdpartyRoot))
    {
      ClangToolsEnabled = false;
    }

    // We read the code generation file in the pre config step
    // so it's only done once (Configure is called for every target)
    ReadCodeGenerationConfigFile();
  }

  // This is called by Sharpmake and acts as the configure entry point.
  // Sharpmake expects different projects to have their own Configure function
  // However, we create our own virtual functions called by base type Configure function.
  // This is meant to make it more scaleable, easier to read and search for specific configuration settings.
  [Configure]
  public void Configure(RexConfiguration conf, RexTarget target)
  {
    BaseConfiguration baseConfig = new BaseConfiguration(this);
    baseConfig.Configure(conf, target);

    // These are private and are not virtualized to be configurable derived projects
    SetupProjectPaths(conf, target);

    // This is private and controlled by this project.
    // derived project should not change the behavior of this
    SetupTestingFlags(conf, target);

    // This is expected to be overriden by derived projects as it's abstract
    SetupOutputType(conf, target);

    // These are protected and optionally changed by derived projects
    SetupConfigSettings(conf, target);
    SetupSolutionFolder(conf, target);

    // These are protected and optionally extended by derived projects
    SetupIncludePaths(conf, target);
    SetupLibDependencies(conf, target);
    SetupOptimizationRules(conf, target);
    SetupPlatformRules(conf, target);
    SetupCompilerRules(conf, target);
    SetupConfigRules(conf, target);
    SetupPostBuildEvents(conf, target);
  }

  #region CppProjectFunctions

  // Specify the targets for this project, meant to be called by every project inherited from this class.
  // The targets for this project are based on the generation settings that are setup by the config file passed in to sharpmake.
  public void GenerateTargets()
  {
    AddTargets(RexTarget.CreateTargets().ToArray());
  }
  // Specify the library dependencies of this project.
  // Library paths, library files and other sharpmake project dependencies are set here.
  protected virtual void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    // To make sure we use the exact same libs, regardless of the compiler
    // We add them here. Clang can figure them out on its own, but it can possibly
    // use updated lib files which could break compilation.
    // This would also cause inconsistency between compilers
    List<string> libPaths = new List<string>();
    libPaths.AddRange(ToolPaths["windows_sdk_lib"].ToList());
    libPaths.AddRange(ToolPaths["msvc_libs"].ToList());
    foreach (var path in libPaths)
    {
      conf.LibraryPaths.Add(path);
    }

    // Add the dependency to the regenerate project for all C++ projects.
    if (target.DevEnv == DevEnv.vs2019)
    {
      conf.AddPublicDependency<RegenerateProjects>(target, DependencySetting.OnlyBuildOrder);
    }
  }

  protected virtual void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    conf.disable_exceptions();

    // Keep in mind, visual studio takes care of its dependencies
    // If project A depends on project B, visual studio will
    // call the build command for project B before
    // it calls the build command for project A

    // So what's the expected behavior we want?
    // If a static library gets selected for building, we want to build that lib and only that lib
    // If a non-static lib gets selected for building, we want to build its dependencies first, and then the non-static lib
    // - when this happens, we need to make sure that when its dependencies change, it needs to relink as well

    // If we're generating make files, we don't use MSBuild as a toolchain
    // but use our own.
    // If we use our own toolchain, we make it possible that you'll build 
    // the same binaries in Visual Studio as you would in VSCode.
    // These would also be the same binaries generated by CI, making everything be in sync.
    bool generatingMakeFiles = true;
    if (generatingMakeFiles)
    {
      // rex python script at the root is the entry point and interface with the rex pipeline
      string rexpyPath = Path.Combine(Globals.Root, "_rex.py");
      
      // Because Visual Studio takes care of the dependency chain, we have to pass in the argument to not build the dependencies
      // We need to somehow configure the paths so that the visual studio projects are pointing correctly
      // but we still use the ninja files that are located elsewhere.
      conf.CustomBuildSettings = new Configuration.NMakeBuildSettings();
      conf.CustomBuildSettings.BuildCommand = $"py {rexpyPath} build -project={Name} -config={target.Config} -compiler={target.Compiler} -dont_build_dependencies";
      conf.CustomBuildSettings.RebuildCommand = $"py {rexpyPath} build -clean -project={Name} -config={target.Config} -compiler={target.Compiler} -dont_build_dependencies";
      conf.CustomBuildSettings.CleanCommand = $"py {rexpyPath} build -nobuild -clean -project={Name} -config={target.Config} -compiler={target.Compiler} -dont_build_dependencies";
      conf.CustomBuildSettings.OutputFile = Path.Combine(conf.TargetPath, conf.TargetFileFullNameWithExtension);
    }

    // Compiler options
    conf.Options.Add(Options.Vc.Compiler.SupportJustMyCode.No); // this adds a call to __CheckForDebuggerJustMyCode into every function that slows down runtime significantly
    conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP17);
    conf.Options.Add(Options.Vc.Compiler.RTTI.Disable);
    conf.Options.Add(Options.Vc.Compiler.RuntimeChecks.Default);
    conf.Options.Add(Options.Vc.Compiler.FloatingPointModel.Fast);
    conf.Options.Add(Options.Vc.Compiler.MultiProcessorCompilation.Enable);
    conf.Options.Add(Options.Vc.Compiler.StringPooling.Enable);
    conf.Options.Add(Options.Vc.Compiler.BufferSecurityCheck.Enable);
    conf.Options.Add(Options.Vc.Compiler.FunctionLevelLinking.Disable);
    conf.Options.Add(Options.Vc.Compiler.FloatingPointExceptions.Disable);
    conf.Options.Add(Options.Vc.Compiler.OpenMP.Disable);

    if (!ProjectGen.Settings.UnityBuildsDisabled)
    {
      conf.Options.Add(Options.Vc.Compiler.JumboBuild.Enable);
    }

    // Linker options
    conf.Options.Add(Options.Vc.Linker.LargeAddress.SupportLargerThan2Gb);
    conf.Options.Add(Options.Vc.Linker.GenerateMapFile.Disable);
    conf.Options.Add(Options.Vc.Linker.GenerateManifest.Disable);
    conf.Options.Add(Options.Vc.Linker.TreatLinkerWarningAsErrors.Enable);

    // This allows SSE for CRC when using Clang
    if (target.Compiler == Compiler.Clang)
    {
      conf.AdditionalCompilerOptions.Add("-msse4.2");
    }

    if (conf.Output == Configuration.OutputType.Exe)
    {
      conf.Options.Add(Options.Vc.Linker.GenerateManifest.Enable);
    }
  }

  // Setup the output type of this project
  // This is usually a static lib or an executable
  // This is meant to be overriden by derived projects.
  protected abstract void SetupOutputType(RexConfiguration conf, RexTarget target);

  // Specify the include paths of the project.
  // This is meant to be overriden by derived projects and extended where needed
  protected virtual void SetupIncludePaths(RexConfiguration conf, RexTarget target)
  {
    // To make sure we use the exact same includes, regardless of the compiler
    // We add them here. Clang can figure them out on its own, but it can possibly
    // use updated include files which could break compilation.
    // This would also cause inconsistency between compilers
    if (target.Compiler == Compiler.MSVC)
    {
      List<string> includePaths = new List<string>();
      includePaths.AddRange(ToolPaths["windows_sdk_includes"].ToList());
      includePaths.AddRange(ToolPaths["msvc_includes"].ToList());
      foreach (var path in includePaths)
      {
        conf.IncludeSystemPaths.Add(path);
      }
    }

    // We always add the include folder of the project to its include paths
    conf.IncludePaths.Add($@"{SourceRootPath}\include");
    conf.IncludePrivatePaths.Add($@"{SourceRootPath}\include_private");
  }
  // Setup rules that need to be defined based on optimization settings
  // This usually means adding or removing defines, but other options are available as well.
  // This is meant to be overridden by derived projects and extended where needed
  protected virtual void SetupOptimizationRules(RexConfiguration conf, RexTarget target)
  {
    // Setup differences between debug vs optimized builds
    switch (target.Optimization)
    {
      case Optimization.NoOpt:
        conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabase);

        conf.Options.Add(Options.Vc.Compiler.Optimization.Disable);
        conf.Options.Add(Options.Vc.Compiler.Intrinsic.Disable);
        conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebug);
        conf.Options.Add(Options.Vc.Compiler.Inline.Default);
        conf.Options.Add(Options.Vc.Compiler.FiberSafe.Disable);
        conf.Options.Add(Options.Vc.Compiler.RuntimeChecks.Both);
        conf.Options.Add(Options.Vc.Compiler.MinimalRebuild.Enable);
        conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.Neither);
        conf.Options.Add(Options.Vc.Compiler.OmitFramePointers.Disable);
        conf.Options.Add(Options.Vc.Compiler.FunctionLevelLinking.Enable);

        conf.Options.Add(Options.Vc.Linker.LinkTimeCodeGeneration.Default);
        conf.Options.Add(Options.Vc.Linker.EnableCOMDATFolding.DoNotRemoveRedundantCOMDATs);
        conf.Options.Add(Options.Vc.Linker.CreateHotPatchableImage.Enable);
        conf.Options.Add(Options.Vc.Linker.Incremental.Enable);
        conf.Options.Add(Options.Vc.Linker.GenerateDebugInformation.Enable);
        break;
      case Optimization.FullOptWithPdb:
      case Optimization.FullOpt:
        conf.Options.Add(Options.Vc.Compiler.Optimization.MaximizeSpeed);
        conf.Options.Add(Options.Vc.Compiler.Intrinsic.Enable);
        conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreaded);
        conf.Options.Add(Options.Vc.Compiler.Inline.AnySuitable);
        conf.Options.Add(Options.Vc.Compiler.FiberSafe.Enable);
        conf.Options.Add(Options.Vc.Compiler.RuntimeChecks.Default);

        conf.Options.Add(Options.Vc.Compiler.MinimalRebuild.Enable);
        conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.FastCode);
        conf.Options.Add(Options.Vc.Compiler.FunctionLevelLinking.Enable);
        conf.Options.Add(Options.Vc.Compiler.OmitFramePointers.Enable);

        conf.Options.Add(Options.Vc.Linker.LinkTimeCodeGeneration.UseLinkTimeCodeGeneration);
        conf.Options.Add(Options.Vc.Linker.EnableCOMDATFolding.RemoveRedundantCOMDATs);
        conf.Options.Add(Options.Vc.Linker.Reference.EliminateUnreferencedData);
        break;
    }

    // disable lto to avoid asan odr issues.
    // can't disable them with ASAN_OPTIONS=detect_odr_violation=0 due to unknown bug
    if (conf.NinjaEnableAddressSanitizer)
    {
      conf.Options.Add(Options.Vc.General.WholeProgramOptimization.Disable);
      conf.Options.Add(Options.Vc.Compiler.Optimization.Disable);
    }

    if (conf.NinjaEnableUndefinedBehaviorSanitizer)
    {
      conf.Options.Add(Options.Vc.Compiler.Optimization.Disable);
    }

    // Setup the difference between optimized builds vs shipping builds
    switch (target.Optimization)
    {
      case Optimization.FullOptWithPdb:
        conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabase);   
        conf.Options.Add(Options.Vc.Compiler.OmitFramePointers.Disable);         // Disable so we can have a stack trace
        break;
      case Optimization.FullOpt:
        conf.Options.Add(Options.Vc.General.DebugInformation.Disable);
        conf.Options.Add(Options.Vc.Compiler.OmitFramePointers.Enable);
        break;
    }
  }
  // Setup rules that need to be defined based on the platform
  // This usually means adding or removing defines, but other options are available as well.
  // This is meant to be overriden by derived projects and extended where needed
  protected virtual void SetupPlatformRules(RexConfiguration conf, RexTarget target)
  {
    switch (target.Platform)
    {
      case Platform.win32:
      case Platform.win64:
        conf.add_public_define("REX_PLATFORM_WINDOWS");

        // This manifest enables utf8 for win API -A functions
        // Please see this link for more details
        // https://learn.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page
        // Enabling utf8 for Window's ANSI functions allows us to use utf8 strings directly
        // without converting them to UCS-2 (Window's default code page)
        if (conf.Output == Configuration.OutputType.Exe)
        {
          conf.AdditionalManifestFiles.Add(Path.Combine(Globals.Root, "_build", "config", "win", "utf8.manifest"));
        }
        break;
      default:
        break;
    }
  }
  // Setup rules that need to be defined based on the compiler
  // This usually means adding or removing defines, but other options are available as well.
  // This is meant to be overriden by derived projects and extended where needed
  protected virtual void SetupCompilerRules(RexConfiguration conf, RexTarget target)
  {
    switch (target.Compiler)
    {
      case Compiler.MSVC:
        conf.add_public_define("REX_COMPILER_MSVC");
        break;
      case Compiler.Clang:
        conf.add_public_define("REX_COMPILER_CLANG");
        break;
      case Compiler.GCC:
        conf.add_public_define("REX_COMPILER_GCC");
        break;
      default:
        break;
    }
  }
  // Setup rules that need to be defined based on the config
  // This usually means adding or removing defines, but other options are available as well.
  // This is meant to be overriden by derived projects and extended where needed
  protected virtual void SetupConfigRules(RexConfiguration conf, RexTarget target)
  {
    switch (target.Config)
    {
      case Config.debug:
      case Config.debug_opt:
        conf.add_public_define("REX_ENABLE_ASSERTS");
        break;
      case Config.release:
        break;
      case Config.coverage:
      case Config.sanitization:
        ClangToolsEnabled = false;
        break;
    }
  }
  // Setup rules for events that need to get fired after a build has finished.
  // Remember that these need to be in batch format.
  protected virtual void SetupPostBuildEvents(RexConfiguration conf, RexTarget target)
  {
    // setup post build command, for more info, look at post_build.py
    // located in the source directory
    string postbuildCommandScriptPath = Path.Combine(Globals.SourceRoot, $"post_build.py");
    string postbuildCommandArguments = "";
    postbuildCommandArguments += $" -p={Name}";
    postbuildCommandArguments += $" -comp={target.Compiler}";
    postbuildCommandArguments += $" -conf={target.ProjectConfigurationName}";
    postbuildCommandArguments += $" -srcroot={SourceRootPath}";

    postbuildCommandArguments += SetupClangTools(conf, target); ;

    conf.EventPostBuild.Add($"py {postbuildCommandScriptPath}{postbuildCommandArguments}");
  }
  // Set the foldername of the solution folder this project belongs to
  protected virtual void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    // Nothing to implement
  }
  #endregion

  // Sets up the project to use clang tools when enabled.
  // Returns the commandline arguments to be passed to post_build.py
  // to enable these tools.
  private string SetupClangTools(RexConfiguration conf, RexTarget target)
  {
    // We can only run clang-tools if the compiler is clang
    if (target.Compiler != Compiler.Clang)
    {
      return "";
    }

    // Don't setup if they're disabled by the derived project
    if (!ClangToolsEnabled)
    {
      // Clean up, it's possible it was enabled in earlier runs of sharpmake
      DeleteClangToolsFolder(conf);
      return "";
    }

    // Prepare to generate the compiler db and copy the config files over
    string compilerDBPath = Utils.GetCompilerDBOutputFolder(conf);
    QueueCompilerDatabaseGeneration(conf);
    CopyClangToolConfigFiles(compilerDBPath);

    // create the extra arguments to be passed in to post_build.py
    string post_build_command = "";
    post_build_command += $" -compdb={compilerDBPath}";
    post_build_command += $" -use_clang_tools";
    post_build_command += $" -clang_tidy_regex=\"{ProjectGen.Settings.ClangTidyRegex}\"";
    if (ProjectGen.Settings.PerformAllClangTidyChecks)
    {
      post_build_command += $" -perform_all_checks";
    }

    return post_build_command;
  }

  // Set testing flags based on configuration specified by the user when calling Sharpmake.
  private void SetupTestingFlags(RexConfiguration conf, RexTarget target)
  {
    if (ProjectGen.Settings.CoverageEnabled)
    {
      conf.NinjaGenerateCodeCoverage = true;
    }

    if (ProjectGen.Settings.AsanEnabled)
    {
      conf.NinjaEnableAddressSanitizer = true;
    }

    if (ProjectGen.Settings.UbsanEnabled)
    {
      conf.NinjaEnableUndefinedBehaviorSanitizer = true;
    }

    if (ProjectGen.Settings.FuzzyTestingEnabled)
    {
      conf.NinjaEnableFuzzyTesting = true;
    }
  }

  // Delete the clang tools output folder if there is one.
  private void DeleteClangToolsFolder(RexConfiguration conf)
  {
    string clangToolsPath = Utils.GetClangToolsOutputFolder(conf);
    if (Directory.Exists(clangToolsPath))
    {
      Directory.Delete(clangToolsPath, recursive: true);
    }
  }

  // This is called by Sharpmake itself after all the projects are created
  public override void PostLink()
  {
    base.PostLink();

    foreach (Configuration config in Configurations)
    {
      RexConfiguration rexConfig = config as RexConfiguration;
      RexTarget rexTarget = config.Target as RexTarget;

      GenerateClangToolProjectFile(rexConfig, rexTarget);
    }
  }

  // Creates the clang tools project file
  // This file is read by the post build step if clang-tools are enabled
  // It's used to build up the commandline to be passed to clang-tools
  private void GenerateClangToolProjectFile(RexConfiguration conf, RexTarget target)
  {
    if (!ClangToolsEnabled)
    {
      return;
    }

    // Clang tools are only enabled if we're using the clang compiler
    if (target.Compiler != Compiler.Clang)
    {
      return;
    }

    string clangToolsProjectPath = Utils.GetCompilerDBOutputFolder(conf);

    // The header filter is a list of regexes we care about when using clang-tidy
    // This is useful for ignoring headers of thirdparty libraries we don't want to run clang-tools on.
    ClangToolsProject project = new ClangToolsProject(Name, clangToolsProjectPath);
    project.HeaderFilters = conf.ClangToolHeaderFilterList.ToList();

    var options = new JsonSerializerOptions
    {
      WriteIndented = true // Makes it a bit more human friendly
    };

    // Write the text to disk in json format
    string jsonBlob = JsonSerializer.Serialize(project, options);

    if (!Directory.Exists(clangToolsProjectPath))
    {
      Directory.CreateDirectory(clangToolsProjectPath);
    }

    File.WriteAllText(project.ProjectPath, jsonBlob);
  }

  // Reads the code generation config file from disk
  // and passes it over to code generation system for processing
  private void ReadCodeGenerationConfigFile()
  {
    string code_generation_config_path = Path.Combine(SourceRootPath, "config", "code_generation.json");

    // Not every project has a code generation config file
    // if one doesn't exists, we early out here
    if (!File.Exists(code_generation_config_path))
    {
      System.Diagnostics.Debug.WriteLine($"Warning: GenerationConfigPath does not exist '{code_generation_config_path}'");
      return;
    }

    // Pass the name of this project and the path to its code generation config over to to code generation system
    // It'll process it internally and the code is automatically generated
    // Code is generated after the projects are created.
    // We cannot know which projects we want to generate until the generation step
    // And therefore we cannot know what we have to autogenerate until that step has finished.
    // So the target files exist in version control and are automatically added to the project
    // but their content is only filled in after the project and solution has been generated.
    CodeGeneration.ReadGenerationFile(Name, code_generation_config_path);
  }

  // Queue up the command for compiler db generation.
  // This is done after the full generation has finished as we won't know
  // all the commandline arguments until the end.
  // We use ninja for the compiler db generation as well so we only have to maintain our ninja files
  // and we can get the compiler db generation for free
  private void QueueCompilerDatabaseGeneration(RexConfiguration config)
  {
    string ninja_file_path = GetNinjaFilePath(config);
    string build_step_name = $"compdb_{Name.ToLower()}_{config.Name}_clang";
    string outputPath = Utils.GetCompilerDBOutputPath(config);

    lock (LockToCompilerDBGenerationQueue)
    {
      ProjectGen.Settings.GenerateCompilerDBCommands.Add(new ProjectGen.GenerateCompilerDBCommand(ninja_file_path, build_step_name, outputPath));
    }
  }

  // To make clang tool processing easier
  // We copy the config files for clang tools next to the compiler db
  // We explore the source root path of each project and walk the parent
  // looking for the various config files.
  // The first that's found will be used and copied over
  private void CopyClangToolConfigFiles(string compilerDBPath)
  {
    string clang_tidy_first_pass_filename = ".clang-tidy_first_pass";
    string clang_tidy_second_pass_filename = ".clang-tidy_second_pass";
    string clang_format_filename = ".clang-format";
    string iwyu_filename = "iwyu.imp";

    string clang_tidy_first_pass_src_path = Path.Combine(Utils.FindInParent(SourceRootPath, clang_tidy_first_pass_filename), clang_tidy_first_pass_filename);
    string clang_tidy_second_pass_src_path = Path.Combine(Utils.FindInParent(SourceRootPath, clang_tidy_second_pass_filename), clang_tidy_second_pass_filename);
    string clang_format_src_path = Path.Combine(Utils.FindInParent(SourceRootPath, clang_format_filename), clang_format_filename);
    string iwyu_src_path = Path.Combine(Utils.FindInParent(SourceRootPath, iwyu_filename), iwyu_filename);

    string clang_tidy_first_pass_dst_path = Path.Combine(compilerDBPath, clang_tidy_first_pass_filename);
    string clang_tidy_second_pass_dst_path = Path.Combine(compilerDBPath, clang_tidy_second_pass_filename);
    string clang_format_dst_path = Path.Combine(compilerDBPath, clang_format_filename);
    string iwyu_dst_path = Path.Combine(compilerDBPath, iwyu_filename);

    if (Directory.Exists(compilerDBPath) == false)
    {
      Directory.CreateDirectory(compilerDBPath);
    }

    File.Copy(clang_tidy_first_pass_src_path, clang_tidy_first_pass_dst_path, true);
    File.Copy(clang_tidy_second_pass_src_path, clang_tidy_second_pass_dst_path, true);
    File.Copy(clang_format_src_path, clang_format_dst_path, true);
    File.Copy(iwyu_src_path, iwyu_dst_path, true);
  }

  // Helper function to get the filepath of the ninja file that'll get generated
  private string GetNinjaFilePath(RexConfiguration config)
  {
    return Path.Combine(config.ProjectPath, "ninja", GetPerConfigFileName(config, config.Target.GetFragment<Compiler>()));
  }
  // Helper function to create a unique filename for the ninja file based on the config
  private string GetPerConfigFileName(RexConfiguration config, Compiler compiler)
  {
    return $"{config.Project.Name}.{config.Target.ProjectConfigurationName}.{compiler}.ninja";
  }

  // Load the json file that specifies all the paths of all the tools needed by rex engine.
  private void LoadToolPaths()
  {
    string tools_json_path = Path.Combine(Globals.LibsRoot, "lib_paths.json");
    string json_blob = File.ReadAllText(tools_json_path);
    ToolPaths = JsonSerializer.Deserialize<Dictionary<string, string[]>>(json_blob);
  }

  // Setup the target path and pdb locations
  private void SetupProjectPaths(RexConfiguration conf, RexTarget target)
  {
    // Because we use ninja files we store binaries and intermediates with the ninja files
    // we hardcode "ninja" as that's the name of the devenv when ninja is selected
    string ninja_files_path = Path.Combine(Globals.BuildFolder, ProjectGen.Settings.IntermediateDir, "ninja", Name);
    conf.TargetPath = Path.Combine(ninja_files_path, "bin", target.ProjectConfigurationName);
    conf.IntermediatePath = Path.Combine(conf.ProjectPath, "intermediate", target.ProjectConfigurationName, target.Compiler.ToString());
    conf.UseRelativePdbPath = false;
    conf.LinkerPdbFilePath = Path.Combine(conf.TargetPath, $"{Name}_{target.ProjectConfigurationName}_{target.Compiler}{conf.LinkerPdbSuffix}.pdb");
    conf.CompilerPdbFilePath = Path.Combine(conf.TargetPath, $"{Name}_{target.ProjectConfigurationName}_{target.Compiler}{conf.CompilerPdbSuffix}.pdb");
  }
}

// This is the base class for every C# project used in the rex solution
// Some of its functionality is sharedwith BasicCPPProject through BaseConfiguration
public abstract class BasicCSProject : CSharpProject
{
  public BasicCSProject() : base(typeof(RexTarget), typeof(RexConfiguration))
  {
    // Nothing to implement
  }

  [Configure]
  public void Configure(RexConfiguration conf, RexTarget target)
  {
    BaseConfiguration baseConfig = new BaseConfiguration(this);
    baseConfig.Configure(conf, target);

    // This is a private func, not to be overridden with derived projects
    SetupProjectPaths(conf, target);

    // These are protected and optionally extended by derived projects
    SetupOutputType(conf, target);
    SetupLibDependencies(conf, target);
    SetupConfigRules(conf, target);
    SetupConfigRules(conf, target);
    SetupPostBuildEvents(conf, target);
    SetupSolutionFolder(conf, target);
  }

  protected abstract void SetupOutputType(RexConfiguration conf, RexTarget target);

  // Setup default configuration settings for C++ projects
  protected virtual void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    // Nothing to implement
  }
  // Specify the library dependencies of this project.
  // Library paths, library files and other sharpmake project dependencies are set here.
  protected virtual void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    // Nothing to implement
  }

  // Setup rules that need to be defined based on the config
  // This usually means adding or removing defines, but other options are available as well.
  // This is meant to be overriden by derived projects and extended where needed
  protected virtual void SetupConfigRules(RexConfiguration conf, RexTarget target)
  {
    // Nothing to implement
  }
  // Setup rules for events that need to get fired after a build has finished.
  // Remember that these need to be in batch format.
  protected virtual void SetupPostBuildEvents(RexConfiguration conf, RexTarget target)
  {
    // Nothing to implement
  }
  // Set the foldername of the solution folder this project belongs to
  protected virtual void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    // Nothing to implement
  }
  // Setup the target path and pdb locations
  private void SetupProjectPaths(RexConfiguration conf, RexTarget target)
  {
    conf.TargetPath = Path.Combine(conf.ProjectPath, "bin", conf.Name);
    conf.IntermediatePath = Path.Combine(conf.ProjectPath, "intermediate", conf.Name, target.Compiler.ToString());
    conf.UseRelativePdbPath = false;
    conf.LinkerPdbFilePath = Path.Combine(conf.TargetPath, $"{Name}_{conf.Name}_{target.Compiler}{conf.LinkerPdbSuffix}.pdb");
    conf.CompilerPdbFilePath = Path.Combine(conf.TargetPath, $"{Name}_{conf.Name}_{target.Compiler}{conf.CompilerPdbSuffix}.pdb");
  }
}

// All projects sitting in 0_thirdparty folder should inherit from this
public class ThirdPartyProject : BasicCPPProject
{
  public ThirdPartyProject() : base()
  {
    // By default we don't enable clang tools for any of our thirdparty projects
    ClangToolsEnabled = false;
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    conf.SolutionFolder = "0_thirdparty";
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Lib;
  }
}

// All projects sitting in 1_engine folder should inherit from this
public class EngineProject : BasicCPPProject
{
  public EngineProject() : base()
  { }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    conf.SolutionFolder = "1_engine";
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Lib;
  }
}

// All projects sitting in 2_platform folder should inherit from this
public class PlatformProject : BasicCPPProject
{
  public PlatformProject() : base()
  { }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    conf.SolutionFolder = "2_platform";
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Lib;
  }
}

// All projects sitting in 3_app_libs folder should inherit from this
public class GameProject : BasicCPPProject
{
  // This is the project name that'll be defined
  // The root project directory will have a folder name 
  // that matches this name
  protected string ProjectName = "project_name";

  public GameProject() : base()
  { }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    conf.SolutionFolder = "3_games";
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
    conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = Path.Combine(Globals.Root, "data");

    if (!Directory.Exists(conf.VcxprojUserFile.LocalDebuggerWorkingDirectory))
    {
      Directory.CreateDirectory(conf.VcxprojUserFile.LocalDebuggerWorkingDirectory);
    }
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Lib;
  }
}

// All projects sitting in 4_tools folder should inherit from this
public class ToolsProject : BasicCPPProject
{
  public ToolsProject() : base()
  { }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    conf.SolutionFolder = "4_tools";
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
    conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = Path.Combine(Globals.Root, "data");

    if (!Directory.Exists(conf.VcxprojUserFile.LocalDebuggerWorkingDirectory))
    {
      Directory.CreateDirectory(conf.VcxprojUserFile.LocalDebuggerWorkingDirectory);
    }
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Lib;
  }
}

// All projects sitting in the tests directory should inherit from this
public class TestProject : BasicCPPProject
{
  // The type of this project
  protected ProjectGen.TestProjectType ProjectType { get; set; }
  // Sharpmake runs multithreaded, so for thread safety, we need to put a lock around accessing the project settings array
  static private object LockToTestProjectSettings = new object();

  public TestProject() : base()
  {
    ProjectType = ProjectGen.TestProjectType.Undefined;
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    conf.SolutionFolder = "5_tests";
  }

  protected override void SetupConfigRules(RexConfiguration conf, RexTarget target)
  {
    switch (target.Config)
    {
      case Config.coverage:
        conf.add_public_define("REX_DISABLE_CATCH"); // we don't need to check catch.
        break;
      case Config.sanitization:
        conf.add_public_define("REX_DISABLE_CATCH"); // we don't need to check catch, it massively increase link time (47min at time of writing -> 5min when disabled)
        break;
      default:
        break;
    }
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
    conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = Path.Combine(Globals.Root, "data", Name);

    if (!Directory.Exists(conf.VcxprojUserFile.LocalDebuggerWorkingDirectory))
    {
      Directory.CreateDirectory(conf.VcxprojUserFile.LocalDebuggerWorkingDirectory);
    }
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Exe;
  }

  protected override void PostInvokeConfiguration()
  {
    base.PostInvokeConfiguration();

    if (ProjectType != ProjectGen.TestProjectType.Undefined)
    {
      lock (LockToTestProjectSettings)
      {
        ProjectGen.Settings.TestProjectsFile.AddProject(ProjectType, this);
      }
    }
  }
}
