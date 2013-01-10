@echo off

set /p pwd=Please input password: <nul
for /f "delims=" %%i in ('%~dp0getpass') do set pwd=%%i

echo "pwd=%pwd%"