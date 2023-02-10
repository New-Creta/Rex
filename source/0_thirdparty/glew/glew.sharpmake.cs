using Sharpmake;
using System.IO;
using System.Diagnostics;

[Export]
public class GLEW : ThirdPartyProject
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
    SourceRootPath = ThisFileFolder;
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.IncludePaths.Add(SourceRootPath);
    conf.Options.Remove(Options.Vc.General.TreatWarningsAsErrors.Enable);

    string library_path = Path.Combine(SourceRootPath, "lib");

    switch(conf.Platform)
    {
      case Platform.win64:
        if (target.Config == Config.release)
        {
          string release_library_path = Path.Combine(library_path, "Release");
          conf.LibraryPaths.Add(Path.Combine(release_library_path, "x64"));
          conf.LibraryFiles.Add("glew32s.lib");
          conf.LibraryFiles.Add("Opengl32.lib");
          Debug.WriteLine(release_library_path);
        }
        else
        {
          string debug_library_path = Path.Combine(library_path, "Debug");
          conf.LibraryPaths.Add(Path.Combine(debug_library_path, "x64"));
          conf.LibraryFiles.Add("glew32sd.lib");
          conf.LibraryFiles.Add("Opengl32.lib");
          Debug.WriteLine(debug_library_path);
        }
        break;
      default:
        Debug.WriteLine("Error: Unsupported platform");
        return;
    }   
  }
}