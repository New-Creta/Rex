import os

if __name__ == "__main__":
    # working directory where we start our script
    cwd = os.getcwd()        

    # add new submodules here that need to be updated when we run this script
    # current submodules updated
    #       * rex-stl
    submodules = [cwd + "/../externals/rex-stl"]

    for submodule in submodules:
        # print the submodule that needs to be updated
        print("\t" + submodule)
        # update given submodule
        os.system("git submodule update --remote " + submodule)