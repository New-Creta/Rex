using Sharpmake;

[Fragment, System.Flags]
public enum Config
{
  assert = (1 << 0),
  debug = (1 << 1),
  debug_opt = (1 << 2),
  release = (1 << 3),
  tests = (1 << 4),
  coverage = (1 << 5),
  address_sanitizer = (1 << 6),
  undefined_behavior_sanitizer = (1 << 7),
  fuzzy = (1 << 8)
}

[Fragment, System.Flags]
public enum Optimization
{
  NoOpt = (1 << 0),
  FullOptWithPdb = (1 << 1),
  FullOpt = (1 << 2)
}

public class RexConfiguration : Sharpmake.Project.Configuration
{
  public RexConfiguration()
  {
  }

  public void enable_exceptions()
  {
    Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);
    Defines.Add("_HAS_EXCEPTIONS=1");
  }

  public void disable_exceptions()
  {
    Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Disable);
    Defines.Add("_HAS_EXCEPTIONS=0");
  }

  public void use_general_options()
  {
    //Options.Add(Sharpmake.Options.Vc.General.JumboBuild.Enable);
    Options.Add(Sharpmake.Options.Vc.General.CharacterSet.MultiByte);
    Options.Add(Sharpmake.Options.Vc.General.PlatformToolset.v142);
    //Options.Add(Sharpmake.Options.Vc.General.VCToolsVersion.v14_21_27702);
    Options.Add(Sharpmake.Options.Vc.General.WarningLevel.Level4);
    Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Enable);
  }

  public void use_compiler_options()
  {
    Options.Add(Sharpmake.Options.Vc.Compiler.SupportJustMyCode.No); // this adds a call to __CheckForDebuggerJustMyCode into every function that slows down runtime significantly
    Options.Add(Sharpmake.Options.Vc.Compiler.CppLanguageStandard.CPP17);
    Options.Add(Sharpmake.Options.Vc.Compiler.RTTI.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeChecks.Default);
    Options.Add(Sharpmake.Options.Vc.Compiler.FloatingPointModel.Fast);
    Options.Add(Sharpmake.Options.Vc.Compiler.MultiProcessorCompilation.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.StringPooling.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.BufferSecurityCheck.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.FunctionLevelLinking.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.FloatingPointExceptions.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.OpenMP.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.JumboBuild.Enable);
  }

  public void use_linker_options()
  {
    Options.Add(Sharpmake.Options.Vc.Linker.LargeAddress.SupportLargerThan2Gb);
    Options.Add(Sharpmake.Options.Vc.Linker.GenerateMapFile.Disable);
    Options.Add(Sharpmake.Options.Vc.Linker.GenerateManifest.Disable);
    Options.Add(Sharpmake.Options.Vc.Linker.TreatLinkerWarningAsErrors.Enable);
  }

  public void enable_optimization()
  {
    Options.Add(Sharpmake.Options.Vc.General.WholeProgramOptimization.LinkTime);

    Options.Add(Sharpmake.Options.Vc.Compiler.Optimization.MaximizeSpeed);
    Options.Add(Sharpmake.Options.Vc.Compiler.Intrinsic.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreaded);
    Options.Add(Sharpmake.Options.Vc.Compiler.Inline.AnySuitable);
    Options.Add(Sharpmake.Options.Vc.Compiler.FiberSafe.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeChecks.Default);

    Options.Add(Sharpmake.Options.Vc.Compiler.MinimalRebuild.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.FavorSizeOrSpeed.FastCode);
    Options.Add(Sharpmake.Options.Vc.Compiler.FunctionLevelLinking.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.OmitFramePointers.Enable);

    Options.Add(Sharpmake.Options.Vc.Linker.LinkTimeCodeGeneration.UseLinkTimeCodeGeneration);
    Options.Add(Sharpmake.Options.Vc.Linker.EnableCOMDATFolding.RemoveRedundantCOMDATs);
    Options.Add(Sharpmake.Options.Vc.Linker.Reference.EliminateUnreferencedData);
    //Options.Add(Sharpmake.Options.Vc.Linker.Incremental.Enable);
  }

  public void disable_optimization()
  {
    Defines.Add("USING_DEBUG_RUNTIME_LIBS");

    Options.Add(Sharpmake.Options.Vc.Compiler.Optimization.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.Intrinsic.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebug);
    Options.Add(Sharpmake.Options.Vc.Compiler.Inline.Default);
    Options.Add(Sharpmake.Options.Vc.Compiler.FiberSafe.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeChecks.Both);
    Options.Add(Sharpmake.Options.Vc.Compiler.MinimalRebuild.Enable);
    Options.Add(Sharpmake.Options.Vc.Compiler.FavorSizeOrSpeed.Neither);
    Options.Add(Sharpmake.Options.Vc.Compiler.OmitFramePointers.Disable);
    Options.Add(Sharpmake.Options.Vc.Compiler.FunctionLevelLinking.Enable);

    Options.Add(Sharpmake.Options.Vc.Linker.LinkTimeCodeGeneration.Default);
    Options.Add(Sharpmake.Options.Vc.Linker.EnableCOMDATFolding.DoNotRemoveRedundantCOMDATs);
    Options.Add(Sharpmake.Options.Vc.Linker.CreateHotPatchableImage.Enable);
    Options.Add(Sharpmake.Options.Vc.Linker.Incremental.Enable);
    Options.Add(Sharpmake.Options.Vc.Linker.GenerateDebugInformation.Enable);
  }

  public void add_public_define(string define)
  {
    Defines.Add(define);
    ExportDefines.Add(define);
  }

  public void set_precomp_header(string projectFolderName, string preCompHeaderName)
  {
    PrecompHeader = projectFolderName + @"/" + preCompHeaderName + @".h";
    PrecompSource = preCompHeaderName + @".cpp";
  }

  public bool is_config_for_testing()
  {
    RexTarget rex_target = Target as RexTarget;
    switch (rex_target.Config)
    {
      case Config.tests:
      case Config.coverage:
      case Config.address_sanitizer:
      case Config.undefined_behavior_sanitizer:
      case Config.fuzzy:
        return true;

      case Config.assert:
      case Config.debug:
      case Config.debug_opt:
      case Config.release:
      default:
        return false;
    }
  }
}

public class ConfigManager
{
  public static Config get_all_configs()
  {
    return Config.assert | Config.debug | Config.debug_opt | Config.release | Config.tests;
  }

  public static Optimization get_optimization_for_config(Config config)
  {
    switch (config)
    {
      case Config.assert: return Optimization.FullOptWithPdb;
      case Config.debug: return Optimization.NoOpt;
      case Config.coverage: return Optimization.NoOpt;
      case Config.debug_opt: return Optimization.FullOptWithPdb;
      case Config.address_sanitizer: return Optimization.FullOptWithPdb;
      case Config.undefined_behavior_sanitizer: return Optimization.FullOptWithPdb;
      case Config.fuzzy: return Optimization.FullOptWithPdb;
      case Config.release: return Optimization.FullOpt;
      case Config.tests: return Optimization.NoOpt;
    }
    return Optimization.FullOpt;
  }
}
