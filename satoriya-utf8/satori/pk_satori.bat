@echo off
set _UPX_PATH=C:\progra~1\univer~1\bin\upx
set _7Z_PATH=C:\progra~1\univer~1\bin\7z

::BEGIN
strip satori.dll
strip ssu.dll

%_UPX_PATH% --best --lzma --ultra-brute satori.dll
%_UPX_PATH% --best --lzma --ultra-brute ssu.dll

%_7Z_PATH% a -tzip -scsWIN -mx=9 -mmt=2 -mfb=258 satori.zip satori.dll ssu.dll

::END
set _UPX_PATH=
set _7Z_PATH=