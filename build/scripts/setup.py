import install_modules
import download_tools
import verify
import input

def main():
  install_modules.run()
  
  if not verify.verify_tools():
    download_tools.run()

  # Tools are now download to build/tools
  # The user can relocate these to a different folder that's in PATH and should
  
    
if __name__ == "__main__":
  main()