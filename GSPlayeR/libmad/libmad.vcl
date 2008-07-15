<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: libmad - Win32 (WCE ARMV4) ReleaseXScale--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP89.tmp" with contents
[
/nologo /W3  /Ox /Ot /Oa /Og /Oi /Ob2 /I "../include" /D "NDEBUG" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "FPM_DEFAULT" /D "OPT_ACCURACY" /Fp"ARMV4ReleaseXScale/libmad.pch" /YX /Fo"ARMV4ReleaseXScale/" /MC /QRxscale /QRxscalesched /c 
"F:\jatf\RTOSS\GSPlayeR\libmad\bit.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\fixed.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\frame.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\huffman.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\layer12.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\layer3.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\libmad.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\stream.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\synth.c"
"F:\jatf\RTOSS\GSPlayeR\libmad\timer.c"
]
Creating command line "clarm.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP89.tmp" 
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP8A.tmp" with contents
[
/nologo /out:"..\lib\ARMV4Relx\libmad.lib" 
.\ARMV4ReleaseXScale\bit.obj
.\ARMV4ReleaseXScale\fixed.obj
.\ARMV4ReleaseXScale\frame.obj
.\ARMV4ReleaseXScale\huffman.obj
.\ARMV4ReleaseXScale\layer12.obj
.\ARMV4ReleaseXScale\layer3.obj
.\ARMV4ReleaseXScale\libmad.obj
.\ARMV4ReleaseXScale\stream.obj
.\ARMV4ReleaseXScale\synth.obj
.\ARMV4ReleaseXScale\timer.obj
]
Creating command line "link.exe -lib @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP8A.tmp"
<h3>Output Window</h3>
Compiling...
bit.c
fixed.c
f:\jatf\rtoss\gsplayer\libmad\fixed.c(63) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
frame.c
f:\jatf\rtoss\gsplayer\libmad\frame.c(229) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\frame.c(318) : warning C4018: '<' : signed/unsigned mismatch
f:\jatf\rtoss\gsplayer\libmad\frame.c(403) : warning C4018: '>' : signed/unsigned mismatch
huffman.c
layer12.c
f:\jatf\rtoss\gsplayer\libmad\layer12.c(164) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer12.c(406) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer12.c(413) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer12.c(421) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer12.c(444) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer12.c(454) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer12.c(459) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
layer3.c
f:\jatf\rtoss\gsplayer\libmad\layer3.c(527) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(536) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(537) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(538) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(539) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(550) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(567) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(574) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(580) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(582) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(583) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(648) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(732) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(736) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(749) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(758) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(767) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(776) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\layer3.c(1056) : warning C4018: '<' : signed/unsigned mismatch
f:\jatf\rtoss\gsplayer\libmad\layer3.c(1091) : warning C4018: '<' : signed/unsigned mismatch
libmad.c
f:\jatf\rtoss\gsplayer\libmad\libmad.c(212) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(213) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(215) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(216) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(218) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(219) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(220) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(221) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(239) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(241) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(243) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libmad\libmad.c(244) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
stream.c
synth.c
f:\jatf\rtoss\gsplayer\libmad\synth.c(599) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
f:\jatf\rtoss\gsplayer\libmad\synth.c(630) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
f:\jatf\rtoss\gsplayer\libmad\synth.c(736) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
f:\jatf\rtoss\gsplayer\libmad\synth.c(768) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
timer.c
Creating library...




<h3>Results</h3>
libmad.lib - 0 error(s), 47 warning(s)
</pre>
</body>
</html>
