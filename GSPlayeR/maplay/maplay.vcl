<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: maplay - Win32 (WCE ARMV4) ReleaseXScale--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPEF.tmp" with contents
[
/nologo /W3 /Oxt /Ob2 /I "../include" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "NDEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /FR"ARMV4ReleaseXScale/" /Fp"ARMV4ReleaseXScale/maplay.pch" /YX /Fo"ARMV4ReleaseXScale/" /MC /QRxscale /QRxscalesched /c 
"F:\jatf\RTOSS\GSPlayeR\maplay\decoder.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\effect.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\mahelper.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\maplay.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\MultiBuff.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\output.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\player.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\PlayerMpg.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\PlayerNet.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\PlayerOv.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\PlayerPlugIn.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\PlayerWav.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\reader.cpp"
"F:\jatf\RTOSS\GSPlayeR\maplay\Receiver.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPEF.tmp" 
<h3>Output Window</h3>
Compiling...
decoder.cpp
effect.cpp
mahelper.cpp
f:\jatf\rtoss\gsplayer\maplay\mahelper.cpp(255) : warning C4018: '<' : signed/unsigned mismatch
f:\jatf\rtoss\gsplayer\maplay\mahelper.cpp(263) : warning C4018: '<' : signed/unsigned mismatch
f:\jatf\rtoss\gsplayer\maplay\mahelper.cpp(278) : warning C4018: '<' : signed/unsigned mismatch
f:\jatf\rtoss\gsplayer\maplay\mahelper.cpp(535) : warning C4018: '!=' : signed/unsigned mismatch
f:\jatf\rtoss\gsplayer\maplay\mahelper.cpp(554) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
maplay.cpp
MultiBuff.cpp
output.cpp
player.cpp
PlayerMpg.cpp
PlayerNet.cpp
PlayerOv.cpp
PlayerPlugIn.cpp
PlayerWav.cpp
reader.cpp
Receiver.cpp
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPF0.tmp" with contents
[
/nologo /out:"..\lib\ARMV4Relx\maplay.lib" 
.\ARMV4ReleaseXScale\decoder.obj
.\ARMV4ReleaseXScale\effect.obj
.\ARMV4ReleaseXScale\mahelper.obj
.\ARMV4ReleaseXScale\maplay.obj
.\ARMV4ReleaseXScale\MultiBuff.obj
.\ARMV4ReleaseXScale\output.obj
.\ARMV4ReleaseXScale\player.obj
.\ARMV4ReleaseXScale\PlayerMpg.obj
.\ARMV4ReleaseXScale\PlayerNet.obj
.\ARMV4ReleaseXScale\PlayerOv.obj
.\ARMV4ReleaseXScale\PlayerPlugIn.obj
.\ARMV4ReleaseXScale\PlayerWav.obj
.\ARMV4ReleaseXScale\reader.obj
.\ARMV4ReleaseXScale\Receiver.obj
]
Creating command line "link.exe -lib @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPF0.tmp"
<h3>Output Window</h3>
Creating library...
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPF1.tmp" with contents
[
/nologo /o"ARMV4ReleaseXScale/maplay.bsc" 
.\ARMV4ReleaseXScale\decoder.sbr
.\ARMV4ReleaseXScale\effect.sbr
.\ARMV4ReleaseXScale\mahelper.sbr
.\ARMV4ReleaseXScale\maplay.sbr
.\ARMV4ReleaseXScale\MultiBuff.sbr
.\ARMV4ReleaseXScale\output.sbr
.\ARMV4ReleaseXScale\player.sbr
.\ARMV4ReleaseXScale\PlayerMpg.sbr
.\ARMV4ReleaseXScale\PlayerNet.sbr
.\ARMV4ReleaseXScale\PlayerOv.sbr
.\ARMV4ReleaseXScale\PlayerPlugIn.sbr
.\ARMV4ReleaseXScale\PlayerWav.sbr
.\ARMV4ReleaseXScale\reader.sbr
.\ARMV4ReleaseXScale\Receiver.sbr]
Creating command line "bscmake.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPF1.tmp"
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
maplay.lib - 0 error(s), 5 warning(s)
</pre>
</body>
</html>
