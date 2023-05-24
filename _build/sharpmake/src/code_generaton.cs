using System.Text.Json;
using Sharpmake;

public class CodeGeneration
{
  public static void Generate(string projectName, string key, string typename, JsonElement content, string generationFilePath)
  {
    switch (typename)
    {
      case "Enum": GenerateEnum(projectName, key, content, generationFilePath); break;
      case "Array": GenerateArray(projectName, key, content, generationFilePath); break;
      default:
        break;
    }
  }

  private static void GenerateEnum(string projectName, string key, JsonElement content, string generationFilePath)
  {
    EnumGenerationConfig enum_config = JsonSerializer.Deserialize<EnumGenerationConfig>(content.ToString());

    if (!GenerateSettings.EnumsToAutoGenerate.ContainsKey(key))
    {
      GenerateSettings.EnumsToAutoGenerate.Add(key, new EnumGenerationSettings());

      // we use the config settings of the first enum we encounter, all others need to match this
      GenerateSettings.EnumsToAutoGenerate[key].ClassName = enum_config.ClassName;
      GenerateSettings.EnumsToAutoGenerate[key].Filepath = enum_config.Filepath;
    }
    else
    {
      EnumGenerationSettings enum_gen_settings = GenerateSettings.EnumsToAutoGenerate[key];

      // class names and filenames should be consistent among all generation files
      if (enum_gen_settings.ClassName != enum_config.ClassName)
      {
        throw new Error($"Enum generation error - unexpected classname: '{enum_config.ClassName}' - expected: {enum_gen_settings.ClassName} for projectName: {projectName}");
      }

      if (enum_gen_settings.Filepath != enum_config.Filepath)
      {
        throw new Error($"Enum generation error - unexpected filepath: '{enum_config.Filepath}' - expected: {enum_gen_settings.Filepath} for projectName: {projectName}");
      }
    }

    EnumGenerationSettings settings = GenerateSettings.EnumsToAutoGenerate[key];
    settings.ProjectToEnumValues.Add(projectName, enum_config.Values);
    settings.ProjectToGenerationFile.Add(projectName, generationFilePath);
  }

  private static void GenerateArray(string projectName, string key, JsonElement content, string generationFilePath)
  {
    ArrayGenerationConfig enum_config = JsonSerializer.Deserialize<ArrayGenerationConfig>(content.ToString());

    if (!GenerateSettings.ArraysToAutoGenerate.ContainsKey(key))
    {
      GenerateSettings.ArraysToAutoGenerate.Add(key, new ArrayGenerationSettings());

      // we use the config settings of the first enum we encounter, all others need to match this
      GenerateSettings.ArraysToAutoGenerate[key].ElementType = enum_config.ElementType;
      GenerateSettings.ArraysToAutoGenerate[key].Name = enum_config.Name;
      GenerateSettings.ArraysToAutoGenerate[key].Includes = enum_config.Includes;
      GenerateSettings.ArraysToAutoGenerate[key].Filepath = enum_config.Filepath;
    }
    else
    {
      ArrayGenerationSettings array_gen_settings = GenerateSettings.ArraysToAutoGenerate[key];

      // element types, class types includes, and filepaths should be consistent among all generation files
      if (array_gen_settings.ElementType != enum_config.ElementType)
      {
        throw new Error($"Array generation error - unexpected element type: '{enum_config.ElementType}' - expected: {array_gen_settings.ElementType} for projectName: {projectName}");
      }

      if (array_gen_settings.Name != enum_config.Name)
      {
        throw new Error($"Array generation error - unexpected name: '{enum_config.Name}' - expected: {array_gen_settings.Name} for projectName: {projectName}");
      }

      if (array_gen_settings.Filepath != enum_config.Filepath)
      {
        throw new Error($"Array generation error - unexpected filepath: '{enum_config.Filepath}' - expected: {array_gen_settings.Filepath} for projectName: {projectName}");
      }
    }

    ArrayGenerationSettings settings = GenerateSettings.ArraysToAutoGenerate[key];
    settings.ProjectToArrayValues.Add(projectName, enum_config.Values);
    settings.ProjectToGenerationFile.Add(projectName, generationFilePath);
  }
}