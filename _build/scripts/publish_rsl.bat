
@REM Push the root directory of REX

set REX_ROOT=%~dp0..\..
set RSL_PUBLISH_DIR=%1
set RSL_DIR=%REX_ROOT%\source\0_thirdparty\rex_std

pushd %RSL_PUBLISH_DIR%

git pull

xcopy %RSL_DIR% %RSL_PUBLISH_DIR% /E /Y

popd