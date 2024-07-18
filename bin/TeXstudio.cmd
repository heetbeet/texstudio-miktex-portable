@echo off
setlocal
set "PATH=%~dp0..\programs\miktex-portable\texmfs\install\miktex\bin\x64;%PATH%"
set "ini_file=%~dp0..\programs\texstudio-4.8.1-win-portable-qt6\texstudio.ini"
set "exe_file=%~dp0..\programs\texstudio-4.8.1-win-portable-qt6\texstudio.exe"

if not exist "%ini_file%" (
    type nul > "%ini_file%"
)

call "%exe_file%" %*
endlocal