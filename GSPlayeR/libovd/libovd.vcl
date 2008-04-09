<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: libovd - Win32 (WCE emulator) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6D.tmp" with contents
[
/nologo /W3 /I "../include" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "_i386_" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "_X86_" /D "x86" /D "NDEBUG" /D "FIXP_SPEED" /D "_LOW_ACCURACY_" /Fp"emulatorRel/libovd.pch" /YX /Fo"emulatorRel/" /Gs8192 /GF /O2 /c 
"F:\jatf\gsp225src\libovd\Tremor\bitwise.c"
"F:\jatf\gsp225src\libovd\Tremor\block.c"
"F:\jatf\gsp225src\libovd\Tremor\codebook.c"
"F:\jatf\gsp225src\libovd\Tremor\floor0.c"
"F:\jatf\gsp225src\libovd\Tremor\floor1.c"
"F:\jatf\gsp225src\libovd\Tremor\framing.c"
"F:\jatf\gsp225src\libovd\Tremor\info.c"
"F:\jatf\gsp225src\libovd\libovd.cpp"
"F:\jatf\gsp225src\libovd\Tremor\mapping0.c"
"F:\jatf\gsp225src\libovd\Tremor\mdct.c"
"F:\jatf\gsp225src\libovd\Tremor\registry.c"
"F:\jatf\gsp225src\libovd\Tremor\res012.c"
"F:\jatf\gsp225src\libovd\Tremor\sharedbook.c"
"F:\jatf\gsp225src\libovd\Tremor\synthesis.c"
"F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c"
"F:\jatf\gsp225src\libovd\Tremor\window.c"
]
Creating command line "cl.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6D.tmp" 
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6E.tmp" with contents
[
/nologo /out:"..\lib\emulatorRel\libovd.lib" 
.\emulatorRel\bitwise.obj
.\emulatorRel\block.obj
.\emulatorRel\codebook.obj
.\emulatorRel\floor0.obj
.\emulatorRel\floor1.obj
.\emulatorRel\framing.obj
.\emulatorRel\info.obj
.\emulatorRel\libovd.obj
.\emulatorRel\mapping0.obj
.\emulatorRel\mdct.obj
.\emulatorRel\registry.obj
.\emulatorRel\res012.obj
.\emulatorRel\sharedbook.obj
.\emulatorRel\synthesis.obj
.\emulatorRel\vorbisfile.obj
.\emulatorRel\window.obj
]
Creating command line "link.exe -lib @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSP6E.tmp"
<h3>Output Window</h3>
Compiling...
bitwise.c
F:\jatf\gsp225src\libovd\Tremor\bitwise.c(85) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
block.c
F:\jatf\gsp225src\libovd\Tremor\block.c(152) : warning C4013: 'calloc2' undefined; assuming extern returning int
F:\jatf\gsp225src\libovd\Tremor\block.c(152) : warning C4047: '=' : 'void *' differs in levels of indirection from 'int '
F:\jatf\gsp225src\libovd\Tremor\block.c(378) : warning C4244: '-=' : conversion from '__int64 ' to 'int ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\block.c(381) : warning C4244: '+=' : conversion from '__int64 ' to 'int ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\block.c(394) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
codebook.c
floor0.c
F:\jatf\gsp225src\libovd\Tremor\floor0.c(350) : warning C4013: 'calloc2' undefined; assuming extern returning int
F:\jatf\gsp225src\libovd\Tremor\floor0.c(346) : warning C4101: 'scale' : unreferenced local variable
floor1.c
F:\jatf\gsp225src\libovd\Tremor\floor1.c(74) : warning C4013: 'calloc2' undefined; assuming extern returning int
framing.c
F:\jatf\gsp225src\libovd\Tremor\framing.c(270) : warning C4244: '=' : conversion from 'unsigned __int32 ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\framing.c(271) : warning C4244: '=' : conversion from 'unsigned __int32 ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\framing.c(272) : warning C4244: '=' : conversion from 'unsigned __int32 ' to 'unsigned char ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\framing.c(273) : warning C4244: '=' : conversion from 'unsigned __int32 ' to 'unsigned char ', possible loss of data
info.c
F:\jatf\gsp225src\libovd\Tremor\info.c(36) : warning C4244: '=' : conversion from 'long ' to 'char ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\info.c(114) : warning C4013: 'calloc2' undefined; assuming extern returning int
libovd.cpp
F:\jatf\gsp225src\libovd\libovd.cpp(490) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
F:\jatf\gsp225src\libovd\libovd.cpp(551) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
F:\jatf\gsp225src\libovd\libovd.cpp(580) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
mapping0.c
F:\jatf\gsp225src\libovd\Tremor\misc.h(197) : warning C4013: '_ilog' undefined; assuming extern returning int
F:\jatf\gsp225src\libovd\Tremor\mapping0.c(88) : warning C4013: 'calloc2' undefined; assuming extern returning int
mdct.c
F:\jatf\gsp225src\libovd\Tremor\misc.h(197) : warning C4013: '_ilog' undefined; assuming extern returning int
registry.c
res012.c
F:\jatf\gsp225src\libovd\Tremor\res012.c(91) : warning C4013: 'calloc2' undefined; assuming extern returning int
sharedbook.c
F:\jatf\gsp225src\libovd\Tremor\misc.h(197) : warning C4013: '_ilog' undefined; assuming extern returning int
F:\jatf\gsp225src\libovd\Tremor\sharedbook.c(190) : warning C4013: 'calloc2' undefined; assuming extern returning int
F:\jatf\gsp225src\libovd\Tremor\sharedbook.c(389) : warning C4244: '=' : conversion from 'long ' to 'char ', possible loss of data
synthesis.c
vorbisfile.c
F:\jatf\gsp225src\libovd\Tremor\misc.h(197) : warning C4013: '_ilog' undefined; assuming extern returning int
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(104) : warning C4244: '+=' : conversion from '__int64 ' to 'int ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(126) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(141) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(151) : warning C4244: 'function' : conversion from '__int64 ' to 'int ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(217) : warning C4244: 'function' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(310) : warning C4244: 'function' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(322) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(358) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(596) : warning C4013: 'calloc2' undefined; assuming extern returning int
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(734) : warning C4244: 'return' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(738) : warning C4244: 'return' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(766) : warning C4244: '=' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(795) : warning C4244: '+=' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(1001) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(1002) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(1016) : warning C4244: '=' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(1038) : warning C4244: '=' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(1193) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(1409) : warning C4244: '=' : conversion from '__int32 ' to 'short ', possible loss of data
window.c
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(151) : warning C4761: integral size mismatch in argument; conversion supplied
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(218) : warning C4761: integral size mismatch in argument; conversion supplied
F:\jatf\gsp225src\libovd\Tremor\vorbisfile.c(310) : warning C4761: integral size mismatch in argument; conversion supplied
F:\jatf\gsp225src\libovd\Tremor\misc.h(197) : warning C4013: '_ilog' undefined; assuming extern returning int
Creating library...




<h3>Results</h3>
libovd.lib - 0 error(s), 49 warning(s)
</pre>
</body>
</html>
