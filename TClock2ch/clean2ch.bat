@echo off

REM
REM リリースバイナリのコピー
REM
copy /y release\*.exe .
copy /y release\*.dll .

REM
REM release/debugフォルダ・suo/aps等を削除
REM
rd  /s /q release debug
del /a:h *.suo
del /s *.aps *.ncb *.vcproj.*.user

REM
REM サブフォルダ以下のrelease/debugフォルダを削除
REM
for /d %%f in (tc2ch\*) do rd /s /q %%f\debug %%f\release

REM pause
echo .

