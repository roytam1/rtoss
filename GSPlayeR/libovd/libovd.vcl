<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: libovd - Win32 (WCE ARMV4) ReleaseXScale--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPBA.tmp" with contents
[
/nologo /W3  /Ox /Ot /Oa /Og /Oi /Ob2 /I "../include" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "NDEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "FIXP_SPEED" /FR"ARMV4ReleaseXScale/" /Fp"ARMV4ReleaseXScale/libovd.pch" /YX /Fo"ARMV4ReleaseXScale/" /MC /QRxscale /QRxscalesched /QRarch5 /c 
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\bitwise.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\block.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\codebook.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\floor0.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\floor1.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\framing.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\info.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\libovd.cpp"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\mapping0.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\mdct.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\registry.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\res012.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\sharedbook.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\synthesis.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\vorbisfile.c"
"F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\window.c"
]
Creating command line "clarm.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPBA.tmp" 
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPBB.tmp" with contents
[
/nologo /out:"..\lib\ARMV4Relx\libovd.lib" 
.\ARMV4ReleaseXScale\bitwise.obj
.\ARMV4ReleaseXScale\block.obj
.\ARMV4ReleaseXScale\codebook.obj
.\ARMV4ReleaseXScale\floor0.obj
.\ARMV4ReleaseXScale\floor1.obj
.\ARMV4ReleaseXScale\framing.obj
.\ARMV4ReleaseXScale\info.obj
.\ARMV4ReleaseXScale\libovd.obj
.\ARMV4ReleaseXScale\mapping0.obj
.\ARMV4ReleaseXScale\mdct.obj
.\ARMV4ReleaseXScale\registry.obj
.\ARMV4ReleaseXScale\res012.obj
.\ARMV4ReleaseXScale\sharedbook.obj
.\ARMV4ReleaseXScale\synthesis.obj
.\ARMV4ReleaseXScale\vorbisfile.obj
.\ARMV4ReleaseXScale\window.obj
]
Creating command line "link.exe -lib @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPBB.tmp"
<h3>Output Window</h3>
Compiling...
Command line warning D4025 : overriding '/QRxscale' with '/QRarch5'
bitwise.c
block.c
f:\jatf\rtoss\gsplayer\libovd\tremor\block.c(152) : warning C4013: 'calloc2' undefined; assuming extern returning int
f:\jatf\rtoss\gsplayer\libovd\tremor\block.c(152) : warning C4047: '=' : 'void *' differs in levels of indirection from 'int '
f:\jatf\rtoss\gsplayer\libovd\tremor\block.c(397) : warning C4244: '-=' : conversion from '__int64 ' to 'int ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\block.c(400) : warning C4244: '+=' : conversion from '__int64 ' to 'int ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\block.c(413) : warning C4244: 'initializing' : conversion from '__int64 ' to 'long ', possible loss of data
codebook.c
floor0.c
f:\jatf\rtoss\gsplayer\libovd\tremor\floor0.c(349) : warning C4013: 'calloc2' undefined; assuming extern returning int
floor1.c
F:\jatf\RTOSS\GSPlayeR\libovd\Tremor\floor1.c(75) : warning C4013: 'calloc2' undefined; assuming extern returning int
framing.c
f:\jatf\rtoss\gsplayer\libovd\tremor\framing.c(36) : warning C4013: 'calloc2' undefined; assuming extern returning int
f:\jatf\rtoss\gsplayer\libovd\tremor\framing.c(36) : warning C4047: 'initializing' : 'struct ogg_buffer_state *' differs in levels of indirection from 'int '
f:\jatf\rtoss\gsplayer\libovd\tremor\framing.c(381) : warning C4244: '=' : conversion from 'unsigned __int32 ' to 'unsigned char ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\framing.c(570) : warning C4047: 'initializing' : 'struct ogg_sync_state *' differs in levels of indirection from 'int '
f:\jatf\rtoss\gsplayer\libovd\tremor\framing.c(825) : warning C4047: 'initializing' : 'struct ogg_stream_state *' differs in levels of indirection from 'int '
info.c
f:\jatf\rtoss\gsplayer\libovd\tremor\info.c(35) : warning C4244: '=' : conversion from 'long ' to 'char ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\info.c(113) : warning C4013: 'calloc2' undefined; assuming extern returning int
libovd.cpp
f:\jatf\rtoss\gsplayer\libovd\libovd.cpp(494) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\libovd.cpp(555) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\libovd.cpp(584) : warning C4244: '=' : conversion from '__int32' to 'short', possible loss of data
mapping0.c
f:\jatf\rtoss\gsplayer\libovd\tremor\mapping0.c(88) : warning C4013: 'calloc2' undefined; assuming extern returning int
mdct.c
registry.c
res012.c
f:\jatf\rtoss\gsplayer\libovd\tremor\res012.c(92) : warning C4013: 'calloc2' undefined; assuming extern returning int
sharedbook.c
f:\jatf\rtoss\gsplayer\libovd\tremor\sharedbook.c(189) : warning C4013: 'calloc2' undefined; assuming extern returning int
f:\jatf\rtoss\gsplayer\libovd\tremor\sharedbook.c(385) : warning C4244: '=' : conversion from 'long ' to 'char ', possible loss of data
synthesis.c
vorbisfile.c
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(442) : warning C4244: 'return' : conversion from '__int64 ' to 'int ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(579) : warning C4244: '=' : conversion from '__int64 ' to 'int ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(666) : warning C4244: 'function' : conversion from '__int64 ' to 'long ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(697) : warning C4013: 'calloc2' undefined; assuming extern returning int
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(697) : warning C4047: '=' : 'struct vorbis_info *' differs in levels of indirection from 'int '
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(698) : warning C4047: '=' : 'struct vorbis_comment *' differs in levels of indirection from 'int '
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(841) : warning C4244: 'return' : conversion from '__int64 ' to 'long ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(845) : warning C4244: 'return' : conversion from '__int64 ' to 'long ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(873) : warning C4244: '=' : conversion from '__int64 ' to 'long ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(1368) : warning C4244: '=' : conversion from '__int64 ' to 'long ', possible loss of data
f:\jatf\rtoss\gsplayer\libovd\tremor\vorbisfile.c(1579) : warning C4244: '=' : conversion from '__int32 ' to 'short ', possible loss of data
window.c
Creating library...
Creating temporary file "C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPBC.tmp" with contents
[
/nologo /o"ARMV4ReleaseXScale/libovd.bsc" 
.\ARMV4ReleaseXScale\bitwise.sbr
.\ARMV4ReleaseXScale\block.sbr
.\ARMV4ReleaseXScale\codebook.sbr
.\ARMV4ReleaseXScale\floor0.sbr
.\ARMV4ReleaseXScale\floor1.sbr
.\ARMV4ReleaseXScale\framing.sbr
.\ARMV4ReleaseXScale\info.sbr
.\ARMV4ReleaseXScale\libovd.sbr
.\ARMV4ReleaseXScale\mapping0.sbr
.\ARMV4ReleaseXScale\mdct.sbr
.\ARMV4ReleaseXScale\registry.sbr
.\ARMV4ReleaseXScale\res012.sbr
.\ARMV4ReleaseXScale\sharedbook.sbr
.\ARMV4ReleaseXScale\synthesis.sbr
.\ARMV4ReleaseXScale\vorbisfile.sbr
.\ARMV4ReleaseXScale\window.sbr]
Creating command line "bscmake.exe @C:\DOCUME~1\Roy\LOCALS~1\Temp\RSPBC.tmp"
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
libovd.lib - 0 error(s), 33 warning(s)
</pre>
</body>
</html>
