cl /c /O1 %~dp0kernelxp_stubs.cpp
%~dp0gsar -sGetProcAddress@8 -rGetProcAddress:x00:x00 -o kernelxp_stubs.obj
%~dp0gsar -sGetModuleHandleA@4 -rGetModuleHandleA:x00:x00 -o kernelxp_stubs.obj
%~dp0gsar -sSetLastError@4 -rSetLastError:x00:x00 -o kernelxp_stubs.obj
%~dp0gsar -sHeapFree@12 -rHeapFree:x00:x00:x00 -o kernelxp_stubs.obj
%~dp0gsar -sHeapAlloc@12 -rHeapAlloc:x00:x00:x00 -o kernelxp_stubs.obj
%~dp0gsar -sGetProcessHeap@0 -rGetProcessHeap:x00:x00 -o kernelxp_stubs.obj
%~dp0gsar -sGetSystemInfo@4 -rGetSystemInfo:x00:x00 -o kernelxp_stubs.obj
link /dll /def:%~dp0kernelxp.def /out:kernelxp.dll /entry:SetDllDirectoryW /force kernelxp_stubs.obj .\kernel32.lib
