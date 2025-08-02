rc %~dp0gsar-res.rc
cl /O2 /D_WIN32 %~dp0gsar.c %~dp0gsarbmg.c %~dp0arg_func.c %~dp0gsar-res.res
