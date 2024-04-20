echo off

rem Run the validation script
py _rex.py validate

rem exit without error if we're good
if %errorlevel%==0 goto Done

rem pause if we do have an error
pause

:Done