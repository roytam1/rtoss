@ECHO OFF
@ECHO Now deleting unused files...
@ECHO.

@IF EXIST "gen_tbar*.aps" DEL "gen_tbar*.aps"
@IF EXIST "gen_tbar*.ncb" DEL "gen_tbar*.ncb"
@IF EXIST "gen_tbar*.opt" DEL "gen_tbar*.opt"
@IF EXIST "gen_tbar*.plg" DEL "gen_tbar*.plg"

@IF EXIST "Debug_2\*.dll" DEL "Debug_2\*.dll"
@IF EXIST "Release_2\*.dll" DEL "Release_2\*.dll"

@IF EXIST "Install\*.dll" DEL "Install\*.dll"

@ECHO Done.
