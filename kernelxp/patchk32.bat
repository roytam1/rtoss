@echo off
for /r %%i in (*.dll;*.exe) do (
 if not %%~nxi.==kernelxp.dll. %~dp0gsar -sKERNEL32.DLL -rKERNELXP.DLL -i -o %%i
)