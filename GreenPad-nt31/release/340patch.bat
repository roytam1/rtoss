rem patching TranslateAcceleratorA
binmay  -i GnPad32s.exe -o GnPad32s.exe.new -s "54 72 61 6E 73 6C 61 74 65 41 63 63 65 6C 65 72 61 74 6F 72 41" -r "54 72 61 6E 73 6C 61 74 65 41 63 63 65 6C 65 72 61 74 6F 72 00"
move /y GnPad32s.exe.new GnPad32s.exe

rem patching IsDialogMessageA
binmay  -i GnPad32s.exe -o GnPad32s.exe.new -s "49 73 44 69 61 6C 6F 67 4D 65 73 73 61 67 65 41" -r "49 73 44 69 61 6C 6F 67 4D 65 73 73 61 67 65 00"
move /y GnPad32s.exe.new GnPad32s.exe

rem patching DragQueryFileA
binmay  -i GnPad32s.exe -o GnPad32s.exe.new -s "44 72 61 67 51 75 65 72 79 46 69 6C 65 41" -r "44 72 61 67 51 75 65 72 79 46 69 6C 65 00"
move /y GnPad32s.exe.new GnPad32s.exe



rem patching TranslateAcceleratorA
binmay  -i GreenPad.exe -o GreenPad.exe.new -s "54 72 61 6E 73 6C 61 74 65 41 63 63 65 6C 65 72 61 74 6F 72 41" -r "54 72 61 6E 73 6C 61 74 65 41 63 63 65 6C 65 72 61 74 6F 72 00"
move /y GreenPad.exe.new GreenPad.exe

rem patching IsDialogMessageA
binmay  -i GreenPad.exe -o GreenPad.exe.new -s "49 73 44 69 61 6C 6F 67 4D 65 73 73 61 67 65 41" -r "49 73 44 69 61 6C 6F 67 4D 65 73 73 61 67 65 00"
move /y GreenPad.exe.new GreenPad.exe

rem patching DragQueryFileA
binmay  -i GreenPad.exe -o GreenPad.exe.new -s "44 72 61 67 51 75 65 72 79 46 69 6C 65 41" -r "44 72 61 67 51 75 65 72 79 46 69 6C 65 00"
move /y GreenPad.exe.new GreenPad.exe
