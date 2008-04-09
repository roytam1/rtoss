<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: libmad - Win32 (WCE emulator) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP68.tmp" with contents
[
/nologo /W3 /I "../include" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "_i386_" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "_X86_" /D "x86" /D "NDEBUG" /D "FPM_DEFAULT" /D "OPT_SPEED" /Fp"emulatorRel/libmad.pch" /YX /Fo"emulatorRel/" /Gs8192 /GF /O2 /c 
"F:\jatf\gsp225src\libmad\bit.c"
"F:\jatf\gsp225src\libmad\fixed.c"
"F:\jatf\gsp225src\libmad\frame.c"
"F:\jatf\gsp225src\libmad\huffman.c"
"F:\jatf\gsp225src\libmad\layer12.c"
"F:\jatf\gsp225src\libmad\layer3.c"
"F:\jatf\gsp225src\libmad\libmad.c"
"F:\jatf\gsp225src\libmad\stream.c"
"F:\jatf\gsp225src\libmad\synth.c"
"F:\jatf\gsp225src\libmad\timer.c"
]
Creating command line "cl.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP68.tmp" 
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP69.tmp" with contents
[
/nologo /out:"..\lib\emulatorRel\libmad.lib" 
.\emulatorRel\bit.obj
.\emulatorRel\fixed.obj
.\emulatorRel\frame.obj
.\emulatorRel\huffman.obj
.\emulatorRel\layer12.obj
.\emulatorRel\layer3.obj
.\emulatorRel\libmad.obj
.\emulatorRel\stream.obj
.\emulatorRel\synth.obj
.\emulatorRel\timer.obj
]
Creating command line "link.exe -lib @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP69.tmp"
<h3>Output Window</h3>
Compiling...
bit.c
fixed.c
F:\jatf\gsp225src\libmad\fixed.c(63) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
frame.c
F:\jatf\gsp225src\libmad\frame.c(229) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
F:\jatf\gsp225src\libmad\frame.c(318) : warning C4018: '<' : signed/unsigned mismatch
F:\jatf\gsp225src\libmad\frame.c(402) : warning C4018: '>' : signed/unsigned mismatch
huffman.c
layer12.c
F:\jatf\gsp225src\libmad\layer12.c(164) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer12.c(382) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer12.c(389) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer12.c(397) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer12.c(420) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer12.c(430) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer12.c(435) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
layer3.c
F:\jatf\gsp225src\libmad\layer3.c(527) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(536) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(537) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(538) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(539) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned short ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(550) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(567) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(574) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(580) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(582) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(583) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(648) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(732) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(736) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(749) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(758) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(767) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(776) : warning C4244: '=' : conversion from 'unsigned long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\layer3.c(1056) : warning C4018: '<' : signed/unsigned mismatch
F:\jatf\gsp225src\libmad\layer3.c(1091) : warning C4018: '<' : signed/unsigned mismatch
libmad.c
F:\jatf\gsp225src\libmad\libmad.c(212) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(213) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(215) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(216) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(218) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(219) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(220) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(221) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(239) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(241) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(243) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libmad\libmad.c(244) : warning C4244: '=' : conversion from 'long ' to 'unsigned char ', possible loss of data
stream.c
synth.c
F:\jatf\gsp225src\libmad\synth.c(599) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
F:\jatf\gsp225src\libmad\synth.c(630) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
F:\jatf\gsp225src\libmad\synth.c(736) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
F:\jatf\gsp225src\libmad\synth.c(768) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
timer.c
Creating library...




<h3>Results</h3>
libmad.lib - 0 error(s), 47 warning(s)
</pre>
</body>
</html>
