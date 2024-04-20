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

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Exe;
    conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    conf.AddPublicDependency<RexWindows>(target);

    conf.LibraryFiles.Add("ProjectedFSLib.lib");
  }
}