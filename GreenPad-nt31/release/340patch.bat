@echo off
echo patching TranslateAcceleratorA in GnPad32s.exe
rem binmay  -i GnPad32s.exe -o GnPad32s.exe.new -s "54 72 61 6E 73 6C 61 74 65 41 63 63 65 6C 65 72 61 74 6F 72 41" -r "54 72 61 6E 73 6C 61 74 65 41 63 63 65 6C 65 72 61 74 6F 72 00"
rem move /y GnPad32s.exe.new GnPad32s.exe
gsar -sTranslateAcceleratorA -rTranslateAccelerator:x00 -o GnPad32s.exe


echo patching IsDialogMessageA in GnPad32s.exe
rem binmay  -i GnPad32s.exe -o GnPad32s.exe.new -s "49 73 44 69 61 6C 6F 67 4D 65 73 73 61 67 65 41" -r "49 73 44 69 61 6C 6F 67 4D 65 73 73 61 67 65 00"
rem move /y GnPad32s.exe.new GnPad32s.exe
gsar -sIsDialogMessageA -rIsDialogMessage:x00 -o GnPad32s.exe

echo patching DragQueryFileA in GnPad32s.exe
rem binmay  -i GnPad32s.exe -o GnPad32s.exe.new -s "44 72 61 67 51 75 65 72 79 46 69 6C 65 41" -r "44 72 61 67 51 75 65 72 79 46 69 6C 65 00"
rem move /y GnPad32s.exe.new GnPad32s.exe
gsar -sDragQueryFileA -rDragQueryFile:x00 -o GnPad32s.exe



echo patching TranslateAcceleratorA in GreenPad.exe
rem binmay  -i GreenPad.exe -o GreenPad.exe.new -s "54 72 61 6E 73 6C 61 74 65 41 63 63 65 6C 65 72 61 74 6F 72 41" -r "54 72 61 6E 73 6C 61 74 65 41 63 63 65 6C 65 72 61 74 6F 72 00"
rem move /y GreenPad.exe.new GreenPad.exe
gsar -sTranslateAcceleratorA -rTranslateAccelerator:x00 -o GreenPad.exe

echo patching IsDialogMessageA in GreenPad.exe
rem binmay  -i GreenPad.exe -o GreenPad.exe.new -s "49 73 44 69 61 6C 6F 67 4D 65 73 73 61 67 65 41" -r "49 73 44 69 61 6C 6F 67 4D 65 73 73 61 67 65 00"
rem move /y GreenPad.exe.new GreenPad.exe
gsar -sIsDialogMessageA -rIsDialogMessage:x00 -o GreenPad.exe

echo patching DragQueryFileA in GreenPad.exe
rem binmay  -i GreenPad.exe -o GreenPad.exe.new -s "44 72 61 67 51 75 65 72 79 46 69 6C 65 41" -r "44 72 61 67 51 75 65 72 79 46 69 6C 65 00"
rem move /y GreenPad.exe.new GreenPad.exe
gsar -sDragQueryFileA -rDragQueryFile:x00 -o GreenPad.exe

pause