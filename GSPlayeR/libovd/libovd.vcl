<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: libovd - Win32 (WCE ARMV4) ReleaseXScale--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6D.tmp" with contents
[
/nologo /W3  /Ox /Ot /Oa /Og /Oi /Ob2 /I "../include" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "NDEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "FIXP_SPEED" /FR"ARMV4ReleaseXScale/" /Fp"ARMV4ReleaseXScale/libovd.pch" /YX /Fo"ARMV4ReleaseXScale/" /MC /QRxscale /QRxscalesched /QRarch5 /c 
"F:\jatf\gsp225src-newTremor\libovd\libovd.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6D.tmp" 
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6E.tmp" with contents
[
/nologo /out:"..\lib\ARMV4Relx\libovd.lib" 
".\ARMV4ReleaseXScale\bitwise.obj"
".\ARMV4ReleaseXScale\block.obj"
".\ARMV4ReleaseXScale\codebook.obj"
".\ARMV4ReleaseXScale\floor0.obj"
".\ARMV4ReleaseXScale\floor1.obj"
".\ARMV4ReleaseXScale\framing.obj"
".\ARMV4ReleaseXScale\info.obj"
".\ARMV4ReleaseXScale\libovd.obj"
".\ARMV4ReleaseXScale\mapping0.obj"
".\ARMV4ReleaseXScale\mdct.obj"
".\ARMV4ReleaseXScale\registry.obj"
".\ARMV4ReleaseXScale\res012.obj"
".\ARMV4ReleaseXScale\sharedbook.obj"
".\ARMV4ReleaseXScale\synthesis.obj"
".\ARMV4ReleaseXScale\vorbisfile.obj"
".\ARMV4ReleaseXScale\window.obj"
]
Creating command line "link.exe -lib @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6E.tmp"
<h3>Output Window</h3>
Compiling...
Command line warning D4025 : overriding '/QRxscale' with '/QRarch5'
libovd.cpp
f:\jatf\gsp225src-newtremor\libovd\libovd.cpp(494) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
f:\jatf\gsp225src-newtremor\libovd\libovd.cpp(555) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
f:\jatf\gsp225src-newtremor\libovd\libovd.cpp(584) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
Creating library...
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6F.tmp" with contents
[
/nologo /o"ARMV4ReleaseXScale/libovd.bsc" 
".\ARMV4ReleaseXScale\bitwise.sbr"
".\ARMV4ReleaseXScale\block.sbr"
".\ARMV4ReleaseXScale\codebook.sbr"
".\ARMV4ReleaseXScale\floor0.sbr"
".\ARMV4ReleaseXScale\floor1.sbr"
".\ARMV4ReleaseXScale\framing.sbr"
".\ARMV4ReleaseXScale\info.sbr"
".\ARMV4ReleaseXScale\libovd.sbr"
".\ARMV4ReleaseXScale\mapping0.sbr"
".\ARMV4ReleaseXScale\mdct.sbr"
".\ARMV4ReleaseXScale\registry.sbr"
".\ARMV4ReleaseXScale\res012.sbr"
".\ARMV4ReleaseXScale\sharedbook.sbr"
".\ARMV4ReleaseXScale\synthesis.sbr"
".\ARMV4ReleaseXScale\vorbisfile.sbr"
".\ARMV4ReleaseXScale\window.sbr"]
Creating command line "bscmake.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6F.tmp"
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
libovd.lib - 0 error(s), 4 warning(s)
</pre>
</body>
</html>
