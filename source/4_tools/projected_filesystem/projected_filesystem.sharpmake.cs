using Sharpmake;
using System.IO;

[Generate]
public class ProjectedFileSystem : ToolsProject
{
  public ProjectedFileSystem() : base()
  {
    Name = "ProjectedFileSystem";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.AddPublicDependency<RexWindows>(target);

    conf.LibraryFiles.Add("ProjectedFSLib.lib");

    conf.Output = Configuration.OutputType.Exe;
    conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
  }
}