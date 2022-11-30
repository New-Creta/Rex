import json
import os

def load_file(path):
  if not os.path.exists(path):
    print("Failed to load json, file does not exist")
    return None

  f = open(path)
  data = json.load(f)
  return data

def save_file(path : str, data):
  f = open(path, "w")
  json.dump(data, f, indent=2)