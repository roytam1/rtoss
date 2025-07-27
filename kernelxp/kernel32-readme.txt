kernel32.lib in this directory is created in this way:
- you need a kernel32.dll from XP RTM
- you need tiny_impdef from TinyCC, or newer version of ntldd
 - for tiny_impdef, run `tiny_impdef kernel32.dll -o kernel32.def` to create kernel32.def
 - for ntldd, run `ntldd --def-output kernel32.dll > kernel32.def` to create kernel32.def
- then you need MSVC's lib.exe for creating this kernel32.lib with following command:
lib /def:kernel32.def /out:kernel32.lib
