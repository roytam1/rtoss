cl /c /O1 kernelxp_stubs.cpp
gsar -sGetProcAddress@8 -rGetProcAddress:x00:x00 -o kernelxp_stubs.obj
gsar -sGetModuleHandleA@4 -rGetModuleHandleA:x00:x00 -o kernelxp_stubs.obj
gsar -sSetLastError@4 -rSetLastError:x00:x00 -o kernelxp_stubs.obj
link /dll /def:kernelxp.def /out:kernelxp.dll /entry:SetDllDirectoryW /force kernelxp_stubs.obj .\kernel32.lib
