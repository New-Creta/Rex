using Sharpmake;
using System.IO;

[Generate]
public class RexLinux : PlatformProject
{
    public RexLinux() : base()
    {
        // The name of the project in Visual Studio. The default is the name of
        // the class, but you usually want to override that.
        Name = "RexLinux";
        GenerateTargets();

        string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
        SourceRootPath = ThisFileFolder;
    }

    public override void Configure(RexConfiguration conf, RexTarget target)
    {
        base.Configure(conf, target);

        conf.Output = Configuration.OutputType.Lib;

        conf.AddPublicDependency<RexEngine>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
        conf.AddPublicDependency<RexOpenGL>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
        conf.AddPublicDependency<RexRendererCore>(target, DependencySetting.Default | DependencySetting.IncludeHeadersForClangtools);
    }
}
