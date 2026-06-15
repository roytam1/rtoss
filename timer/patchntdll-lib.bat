@echo off
"%~dp0gsar" -sRtlUnwind@16:x00:x00 -rRtlUnwind:x00:x00:x00:x00:x00 -i -o "ntdll.lib"
pause