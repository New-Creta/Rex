using Sharpmake;
using System.IO;
using System.Text.Json;
using System.Collections.Generic;
using System.Linq;
using System.Text;

[Generate]
public class RexEngine : EngineProject
{
  private string MemoryTagsHeaderFile;

  public RexEngine() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = "RexEngine";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    SourceFilesExcludeFromJumboRegex.Add("new_delete.cpp"); // needs to be excluded to avoid linker issues

    // Only needs to happen on Windows
    SourceFilesExcludeRegex.Add("unix/*");
    //// Only needs to happen on Linux
    //SourceFilesExcludeRegex.Add("win/*");
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.Output = Configuration.OutputType.Lib;

    conf.AddPublicDependency<RexStdExtra>(target, DependencySetting.Default);
    conf.AddPublicDependency<GLM>(target, DependencySetting.Default);

    if (GenerateSettings.GenerateUnitTests)
    {
      conf.AddPublicDependency<RexStdTest>(target, DependencySetting.Default);
    }

    switch (target.Config)
    {
      case Config.debug:
      case Config.debug_opt:
        conf.add_public_define("REX_ENABLE_MEM_TRACKING");
        goto case Config.address_sanitizer;
      case Config.address_sanitizer:
      case Config.undefined_behavior_sanitizer:
      case Config.fuzzy:
        conf.add_public_define("REX_DEBUG");
        break;
    }


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

    switch (GenerateSettings.GraphicsAPI)
    {
        case GenerationTypes.GraphicsAPI.OpenGL:
            conf.add_public_define("REX_API_OPENGL");
            break;
        case GenerationTypes.GraphicsAPI.DirectX12:
            conf.add_public_define("REX_API_DIRECTX12");
            break;
        default:
            break;
    }

    conf.add_public_define("REXLOG_COMPILED_LIB");
  }

  public override void AfterConfigure()
  {
    base.AfterConfigure();
  }
}