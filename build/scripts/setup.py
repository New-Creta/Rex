import install_modules
import required_tools
import rex_json

def main():
  install_modules.run()
  
  if not required_tools.are_installed():
    required_tools.download()
    required_tools.install()

if __name__ == "__main__":
  main()