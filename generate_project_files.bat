rem Run setup
py _rex.py setup

rem Generate a solution using the Visual Studio as IDE.
rem The generation pipeline will find an installed version of VS and use that one.
py _rex.py generate -use-default-config -IDE VisualStudio

rem exit without error if we're good
if %errorlevel%==0 goto Done

rem pause if we do have an error
pause

:Done