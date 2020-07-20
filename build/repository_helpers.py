import os.path

##-------------------------------------------------------------------------------
## PUBLIC FUNCTIONS

##-------------------------------------------------------------------------------
## Find git executable inside the environment variables
def get_git_executable():
    executable = 'git'

    if os.path.sep in executable:
        raise ValueError("Invalid filename: %s" % executable)

    path = os.environ.get("PATH", "").split(os.pathsep)
    # PATHEXT tells us which extensions an executable may have
    path_exts = os.environ.get("PATHEXT", ".exe;.bat;.cmd").split(";")
    has_ext = os.path.splitext(executable)[1] in path_exts
    if not has_ext:
        exts = path_exts
    else:
        # Don't try to append any extensions
        exts = [""]

    for d in path:
        try:
            for ext in exts:
                exepath = os.path.join(d, executable + ext)
                if os.access(exepath, os.X_OK):
                    return exepath
        except OSError:
            pass

    return None