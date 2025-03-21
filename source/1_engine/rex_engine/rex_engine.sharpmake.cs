using Sharpmake;
using System.IO;

[Generate]
public class RexEngine : EngineProject
{
  public RexEngine() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexEngine";
    GenerateTargets();

    // Add the root where all sources can be found under
    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    SourceFilesExcludeFromJumboRegex.Add("new_delete.cpp"); // needs to be excluded to avoid linker issues

    // Only needs to happen on Windows
    SourceFilesExcludeRegex.Add("unix/*");
    //// Only needs to happen on Linux
    //SourceFilesExcludeRegex.Add("win/*");

    DataPath = Path.Combine(Globals.Root, "data", "rex");
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexStd>(target, DependencySetting.Default);
    conf.AddPublicDependency<GLM>(target, DependencySetting.Default);
    conf.AddPublicDependency<IMGUI>(target, DependencySetting.Default);
    conf.AddPublicDependency<Entt>(target, DependencySetting.Default);

    if (conf.Platform == Platform.win32 || conf.Platform == Platform.win64)
    {
      conf.LibraryFiles.Add("Setupapi.lib");
    }
  }

  protected override void SetupConfigRules(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigRules(conf, target);

    // Defines indicating which config is used
    switch (target.Config)
    {
      case Config.debug:
        conf.add_public_define("REX_BUILD_DEBUG");
        break;
      case Config.debug_opt:
        conf.add_public_define("REX_BUILD_DEBUG_OPT");
        break;
      case Config.release:
        conf.add_public_define("REX_BUILD_RELEASE");
        break;
      case Config.coverage:
        conf.add_public_define("REX_BUILD_COVERAGE");
        break;
      case Config.sanitization:
        conf.add_public_define("REX_BUILD_SANITIZATION");
        break;
      default:
        break;
    }

    // Defines indicating which optimization is used
    switch (target.Optimization)
    {
      case Optimization.NoOpt:
        conf.add_public_define("REX_DISABLE_OPTIMIZATION");
        break;
      case Optimization.FullOptWithPdb:
      case Optimization.FullOpt:
        conf.add_public_define("REX_ENABLE_OPTIMIZATION");
        break;
    }

    // Defines added based on config
    switch (target.Config)
    {
      case Config.debug:
      case Config.debug_opt:
        conf.add_public_define("REX_ENABLE_MEM_TRACKING");
        conf.add_public_define("REX_ENABLE_HR_CALL");
        conf.add_public_define("REX_ENABLE_WIN_CALL");
        goto case Config.sanitization;
      case Config.sanitization:
        break;
    }

    // Set the fuzzy define if we're generating fuzzy tests
    if (ProjectGen.Settings.FuzzyTestingEnabled)
    {
      conf.add_public_define("REX_BUILD_FUZZY");
    }
  }

  protected override void SetupPlatformRules(RexConfiguration conf, RexTarget target)
  {
    base.SetupPlatformRules(conf, target);

    switch (conf.Platform)
    {
      case Platform.win32:
        conf.add_public_define("REX_PLATFORM_X86");
        conf.add_public_define("REX_PLATFORM_WINDOWS");
        conf.SourceFilesBuildExcludeRegex.Add("unix/*"); // exclude unix files
        break;
      case Platform.win64:
        conf.add_public_define("REX_PLATFORM_X64");
        conf.add_public_define("REX_PLATFORM_WINDOWS");
        conf.SourceFilesBuildExcludeRegex.Add("unix/*"); // exclude unix files
        break;
      case Platform.linux:
        conf.add_public_define("REX_PLATFORM_LINUX");
        conf.SourceFilesBuildExcludeRegex.Add("win/*"); // exclude windows files
        break;
      default:
        break;
    }
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.add_public_define("GLM_FORCE_SILENT_WARNINGS");
    
    switch (ProjectGen.Settings.GraphicsAPI)
    {
      case ProjectGen.GraphicsAPI.OpenGL:
        conf.add_public_define("REX_API_OPENGL");
        break;
      case ProjectGen.GraphicsAPI.DirectX12:
        conf.add_public_define("REX_API_DIRECTX12");
            conf.add_public_define("GLM_FORCE_DEPTH_ZERO_TO_ONE");
            conf.add_public_define("GLM_FORCE_LEFT_HANDED");
        break;
      default:
        break;
    }

    if (ProjectGen.Settings.AsanEnabled)
    {
      conf.add_public_define("REX_ASAN");
    }

    if (ProjectGen.Settings.UbsanEnabled)
    {
      conf.add_public_define("REX_UBSAN");
    }

    conf.add_public_define("REXLOG_COMPILED_LIB");
  }
}