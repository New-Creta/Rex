using Sharpmake;
using System.IO;
using System.Diagnostics;

[Export]
public class GLAD : ThirdPartyProject
{
  public GLAD() : base()
  {
    // The name of the project in Visual Studio. The default is the name of
    // the class, but you usually want to override that.
    Name = GenerateName("GLAD");
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
  }
}