using Sharpmake;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Xml.Linq;

namespace rex
{
  // This class descibes a module of rex. 
  // A module is equivalent to a Visual Studio project
  // We have this class so we can save module information to disk post generation
  // so the engine can load it at runtime and use it for initialization
  public static class RexModule
  {
    // Serialize the module to json format
    // Because a module can be configured different based on the configuration
    // we need to serialize it based on configuration as well
    public static string SerializeForConfig(string dataRoot, RexConfiguration conf)
    {
      // Fields to store
      // - name
      // - source root
      // - data root
      // - configs
      //   - name
      //   - ninja file (how it's made)
      //   - output path (if made, where it'd be outputed)
      //   - dependencies (pointing to other projects)
      //   - runtime dependencies (pointing to other projects)
      Dictionary<string, object> fields = new Dictionary<string, object>();

      fields.Add("module_name", conf.Project.Name);
      fields.Add("source_root", conf.Project.SourceRootPath);
      fields.Add("data_root", dataRoot);
      string intermediateTargetPath = conf.Project is CSharpProject ? conf.Target.GetFragment<DotNetFramework>().ToFolderName() : "";
      fields.Add("target_path", Path.Combine(conf.TargetPath, intermediateTargetPath, conf.TargetFileFullNameWithExtension));
      fields.Add("config_name", conf.Name);

      List<string> buildDependencies = new List<string>();
      foreach (Project.Configuration dependency in conf.ConfigurationDependencies)
      {
        RexConfiguration? rexConfig = dependency as RexConfiguration;
        if (rexConfig != null)
        {
          string dependencyPath = PathGeneration.CreateIntermediateModuleFilePath(rexConfig);
          buildDependencies.Add(dependencyPath);
        }
      }
      fields.Add("build_dependencies", buildDependencies);

      List<string> runtimeDependencies = new List<string>();
      foreach (Project.Configuration dependency in conf.RuntimeDependencies)
      {
        RexConfiguration? rexConfig = dependency as RexConfiguration;
        if (rexConfig != null)
        {
          string dependencyPath = PathGeneration.CreateIntermediateModuleFilePath(rexConfig);
          runtimeDependencies.Add(dependencyPath);
        }
      }
      fields.Add("runtime_dependencies", runtimeDependencies);

      string jsonString = JsonSerializer.Serialize(fields, new JsonSerializerOptions()
      {
        WriteIndented = true
      });

      return jsonString;
    }

    // Write the module file of this project
    // A module file is informatino about a module but stripped from a lot of irrelevant data
    // it holds information about the module itself, how it was made and the name of the configuration itself
    public static void WriteModuleFile(string projectDataPath, RexConfiguration conf)
    {
      string moduleAsJson = RexModule.SerializeForConfig(projectDataPath, conf);

      // Write the module file at the intermediate location
      string intermediateModuleFilePath = Path.Combine(conf.IntermediatePath, $"{conf.Project.Name}_{conf.Name}_module.json");
      Utils.SafeWriteFile(intermediateModuleFilePath, moduleAsJson);

      string moduleFilePath = PathGeneration.CreateModuleFilePath(conf);
      // Make sure the directory where the module file would be created exists
      // otherwise we can't create the file
      if (!Directory.Exists(Path.GetDirectoryName(moduleFilePath)))
      {
        Directory.CreateDirectory(Path.GetDirectoryName(moduleFilePath));
      }

      // Copy the module file to the final location after having having build it
      conf.EventPostBuild.Add($"copy {intermediateModuleFilePath} {moduleFilePath} /Y");
    }
  }
}