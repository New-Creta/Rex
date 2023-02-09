using Sharpmake;
using System.IO;
using System.Diagnostics;

[Generate]
public class GLEW : BasicCPPProject
{
  public GLEW() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = GenerateName("GLEW");
    GenerateTargets();

    // The directory that contains the source code we want to build is the
    // same as this one. This string essentially means "the directory of
    // the script you're reading right now."
    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = Path.Combine(ThisFileFolder, @"include");
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.IncludePaths.Add(SourceRootPath);
    conf.Output = Configuration.OutputType.Lib;

    string library_path = Path.Combine(Path.GetDirectoryName(Utils.CurrentFile()), "lib");

    switch(conf.Platform)
    {
      case Platform.win64:
        if (target.Config == Config.release)
        {
          string release_library_path = Path.Combine(library_path, "Release");
          conf.TargetPath = Path.Combine(release_library_path, "x64");
          conf.TargetFileName = "glew32s";
        }
        else
        {
          string debug_library_path = Path.Combine(library_path, "Debug");
          conf.TargetPath = Path.Combine(debug_library_path, "x64");	
          conf.TargetFileName = "glew32sd";
        }
        break;
      default:
        Debug.WriteLine("Error: Unsupported platform");
        return;
    }   
  }
}