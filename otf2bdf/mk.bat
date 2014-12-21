@echo off
set FT2DIR=..\freetype2-master
echo on
cl /O2 /MD /DWIN32 otf2bdf.c remap.c /I %FT2DIR%\include %FT2DIR%\objs\freetype.lib
