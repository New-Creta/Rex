using Sharpmake;
using System.IO;

[Generate]
public class DokanFs : ToolsProject
{
  public DokanFs() : base()
  {
    Name = "DokanFs";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;

    SourceFilesCompileAsCRegex.Add("\\.c\\b"); // everything ending with a .c extension
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.Exe;
    conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<RexWindows>(target);
  }

  protected override void SetupIncludePaths(RexConfiguration conf, RexTarget target)
  {
    base.SetupIncludePaths(conf, target);

    //conf.IncludePaths.Add(@"C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\km\crt", 2);
    //conf.IncludePaths.Add(@"C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\km", 2);
    //conf.IncludePaths.Add(@"C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\shared", 2);
  }

  protected override void SetupConfigSettings(RexConfiguration conf, RexTarget target)
  {
    base.SetupConfigSettings(conf, target);

    conf.SourceFilesBuildExcludeRegex.Add("dokanctl.c");
    conf.SourceFilesBuildExcludeRegex.Add("sys_*");
    conf.Defines.Add("_EXPORTING");

    conf.enable_exceptions(); // This must be enabled in order to compile the driver
    conf.Options.Remove(Options.Vc.Compiler.JumboBuild.Enable);
  }
}