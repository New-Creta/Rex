import os
import rex_json
import util

root = util.find_root()
tool_paths = rex_json.load_file(os.path.join(root, "build", "config", "paths.json"))

def run():
  sharpmake_path = tool_paths["sharpmake_path"]
  
  if len(sharpmake_path) == 0:
    raise Exception("Failed to find sharpmake path")
  
  # command = f"/sources(@'D:/Testing/CodeCoverage/build/sharpmake/src/main.sharpmake.cs') /diagnostics"
  sharpmake_main = os.path.join(root, "build", "sharpmake", "src", "main.sharpmake.cs")
  sharpmake_main = sharpmake_main.replace('\\', '/')
  os.system(f"{sharpmake_path} /sources(\"{sharpmake_main}\") /diagnostics")

if __name__ == "__main__":
  run()