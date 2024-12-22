@echo off
if %1.==console. goto console
cl /O2 cpuid.c /link /subsystem:windows /entry:mainCRTStartup 
goto end

:console
cl /O2 cpuid.c /DCPUID_CONSOLE /Fecpuidc.exe

:end
