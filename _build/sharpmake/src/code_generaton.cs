using System.Collections.Generic;
using System.Text.Json;
using System.IO;
using Sharpmake;
using System.Text;

// THE CODE GENERATION SYSTEM
// Some code in the engine gets auto generated.
// While other engines allow users to extend the headers file, 
// we generate the content of these files from disk.
// The benefit we get from that is that the engine code and project code stays split.
// For example, the engine's memory tags get auto generated,
// the engine has its own memory tags and each project can have their own as well.
// In engines like Unreal, one must change the header file of this enum and change it accordingly for the project
// While this is effective, this means that these enum values get hardcoded into engine code,
// which is not where they belong in our opinion.
// If we use configuration files, yes, they will end up in that header file,
// but where they come from is from the project you're working on
// splitting responsibility from the engine and putting on the custom project.

// Sharpmake reads code generation config files and combines what's found 
// and generates code in the destination files.
// A key is mapped with values and 1 (and only 1) entry can define its type and class name
// this is similar as the ODR rule that many programming languages follow
// Supported type of generation are enum classes and static arrays and their values.
// an example of a config could be
//
// Owning definition
// {
// "MemoryTags": {
//   "Type" : "Enum",
//     "ClassName" : "MemoryTag",
//     "Filepath" : "1_engine/rex_engine/include/rex_engine/memory/memory_tags.h",
//     "Content" : 
//     ["Global", "Engine", "FileIO", "StringPool"]
//   }
// }
//
//
// Non-Owning definition
// {
// "MemoryTags": {
//     "Content" : [
//     "Global", "CustomTag1", "CustomTag2", "CustomTag3"
//     ]
//   }
// }
//
// This would result in the following enum created in the file 1_engine/rex_engine/include/rex_engine/memory/memory_tags.h
// enum class MemoryTag
// {
//    Global,
//    Engine,
//    FileIO,
//    StringPool,
//    CustomTag1,
//    CustomTag2,
//    CustomTag3,
// };
//
//
// WARNING: Because some files get updated based on configuration, when running generation - building - generation
// with different settings, incremental builds get affected as some files might be different, causing the build system
// to retrigger compilation. 
// This mostly affects CI as users don't often do this themselves
//
//


public static class CodeGeneration
{
  static private Dictionary<string, CodeGen.TypeToGenerate> TypesToGenerate = new Dictionary<string, CodeGen.TypeToGenerate>();
  static private Dictionary<string, CodeGen.UnknownTypeConfig> UnknownTypesToGenerate = new Dictionary<string, CodeGen.UnknownTypeConfig>();
  static private object MemberAccessLock = new object();

  // Reads the generation config file and processes it
  public static void ReadGenerationFile(string projectName, string filePath)
  {
    // Deserialize the json document into a dictionary
    string json_blob = File.ReadAllText(filePath);
    Dictionary<string, JsonDocument> config = JsonSerializer.Deserialize<Dictionary<string, JsonDocument>>(json_blob);

    // Loop over the dict by key, for each key, check its key and process accordingly
    foreach (string key in config.Keys)
    {
      JsonDocument doc = config[key];
      Process(projectName, doc, key);
    }
  }

  // Process the generation config file.
  // Either creating a type known to generate or holding on to the values
  // so we can add them to known types later
  private static void Process(string projectName, JsonDocument doc, string key)
  {
    JsonElement contentElement = doc.RootElement.GetProperty("Content");
    List<string> content = JsonSerializer.Deserialize<List<string>>(contentElement.ToString());

    JsonElement type = new JsonElement();
    // If we find a type, we own this type
    // We can start filling in all the missing details
    if (doc.RootElement.TryGetProperty("Type", out type))
    {
      string typename = type.GetString();

      switch (typename)
      {
        case "Enum": 
          ProcessEnum(projectName, doc, content, key);
          break;
        case "Array": 
          ProcessArray(projectName, doc, content, key);
          break;
        default:
          throw new Error($"Unknown typename in code generation: '{typename}' for project '{projectName}' at key '{key}'");
      }
    }
    // If we can't find the type, this is just a key pair with content.
    // We find the rest of the info later but for now store its content
    // so we can add it to the content of the type later
    else
    {
      // Sharpmake runs multithreaded, so we need to make sure we use a mutex here
      // Otherwise generation might fail because of a data race.

      lock (MemberAccessLock)
      {
        // If we already have an enum for this, add the content to 
        if (TypesToGenerate.ContainsKey(key))
        {
          TypesToGenerate[key].AddContent(projectName, content);
        }

        // The type for this key is not known yet, we need to add it to the dict of unknown types
        // If the key doesn't exist yet in that dict, we need to add it first
        else if (!UnknownTypesToGenerate.ContainsKey(key))
        {
          UnknownTypesToGenerate.Add(key, new CodeGen.UnknownTypeConfig());
        }

        // Add the content to the unknown type in the dict
        UnknownTypesToGenerate[key].AddContent(projectName, content);
      }
    }
  }

  // Process am enum type
  private static void ProcessEnum(string projectName, JsonDocument typeDefine, List<string> content, string key)
  {
    // Take the variables out of the json element
    string className = typeDefine.RootElement.GetProperty("ClassName").GetString();
    string filepath = typeDefine.RootElement.GetProperty("Filepath").GetString();

    // Create the type that needs to get generated
    TypesToGenerate.Add(key, new CodeGen.EnumToGenerate(className, filepath, projectName, content));    

    // Add remaining unknown types with the same key to the enum settings
    if (UnknownTypesToGenerate.ContainsKey(key))
    {
      CodeGen.UnknownTypeConfig unknownType = UnknownTypesToGenerate[key];
      foreach (string project in unknownType.ProjectToContent.Keys)
      {
        List<string> unknownTypeContent = unknownType.ProjectToContent[project];
        TypesToGenerate[key].AddContent(project, unknownTypeContent);
      }
    }
  }

  // Process an array type
  private static void ProcessArray(string projectName, JsonDocument typeDefine, List<string> content, string key)
  {
    // Take the variables out of the json element
    string elementType = typeDefine.RootElement.GetProperty("ElementType").GetString();
    string name = typeDefine.RootElement.GetProperty("Name").GetString();
    string filepath = typeDefine.RootElement.GetProperty("Filepath").GetString();
    List<string> includes = new List<string>();
    JsonElement includesElement = new JsonElement();
    if (typeDefine.RootElement.TryGetProperty("Includes", out includesElement))
    {
      includes = JsonSerializer.Deserialize<List<string>>(includesElement.ToString());
    }

    // Create the type that needs to get generated
    TypesToGenerate.Add(key, new CodeGen.ArrayToGenerate(elementType, name, filepath, includes, projectName, content));

    // Add remaining unknown types with the same key to the enum settings
    if (UnknownTypesToGenerate.ContainsKey(key))
    {
      CodeGen.UnknownTypeConfig unknownType = UnknownTypesToGenerate[key];
      foreach (string project in unknownType.ProjectToContent.Keys)
      {
        List<string> unknownTypeContent = unknownType.ProjectToContent[project];
        TypesToGenerate[key].AddContent(project, unknownTypeContent);
      }
    }
  }

  // go over the dict holding the types to generate and generate all of them
  public static void Generate()
  {
    foreach (string key in TypesToGenerate.Keys)
    {
      string typeText = TypesToGenerate[key].AsString(key);
      FileInfo generatedFileInfo = new FileInfo(TypesToGenerate[key].FilePath);

      // To not mess up the build system which checks file stamps
      // only write to disk if the new generated file is actually different
      if (Sharpmake.Util.IsFileDifferent(generatedFileInfo, new MemoryStream(Encoding.UTF8.GetBytes(typeText))))
      {
        WriteToDisk(TypesToGenerate[key].FilePath, typeText);
      }
    }
  }

  // write the type text to disk at the given filepath
  private static void WriteToDisk(string filePath, string text)
  {
    // Open a stream, create the file if it doesn't exist yet
    FileStream stream;
    if (!File.Exists(filePath))
    {
      stream = File.Create(filePath);
    }
    else
    {
      stream = File.Open(filePath, FileMode.Truncate);
    }

    // Load the text into an array of bytes
    byte[] bytes = Encoding.ASCII.GetBytes(text);

    // Write the array of bytes to disk
    stream.Write(bytes, 0, text.Length);
    stream.Close();

  }
}