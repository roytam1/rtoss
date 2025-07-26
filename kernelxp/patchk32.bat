@echo off
for /r %%i in (*.dll;*.exe) do (
 if not %%~nxi.==kernelxp.dll. "%~dp0gsar" -sKERNEL32.DLL -rKERNELXP.DLL -i -o "%%~i"
)
for /r %%i in (api-ms-win-core-*.dll) do (
 "%~dp0gsar" -skernel32. -rkernelxp. -i -o "%%~i"
)