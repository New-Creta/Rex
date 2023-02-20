import argparse
import os
import git_repository

# entry point of the script
if __name__ == "__main__":
    # add different arguemtns to this python script
    parser = argparse.ArgumentParser(description='Fetching parameters.')
    parser.add_argument('-cwd', help='root directory of the script.', default=os.getcwd())

    # parse all arguments
    args = parser.parse_args()
    
    # get given URL
    url = "https://github.com/RisingLiberty/RexPy/archive/refs/heads/main.zip"

    # cwd: given current working directory ( this can be different from the script cwd )
    # cod: given current output directory ( this can be different from the script cwd )
    cwd = args.cwd
    cod = "rexpy"
    
    git_repository.fetch(url,cod,cwd)