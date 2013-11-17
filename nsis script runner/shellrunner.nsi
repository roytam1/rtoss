!include "FileFunc.nsh"
!include "StrFunc.nsh"
!include "LogicLib.nsh"

!packhdr "$%TEMP%\exehead.tmp" 'upx --best "$%TEMP%\exehead.tmp"'

Var SetupName

OutFile "shellrunner.exe"
Name $SetupName
Icon "gear_02.ico"
SetCompressor LZMA

; Comment out next line for ANSI build
Unicode true ; make a Unicode installer


RequestExecutionLevel user

Var scriptname
Var argvc
Var argv0
Var argv1
Var argv2
Var argv3
Var argv4
Var argv5
Var argv6
Var argv7
Var argv8
Var argv9

; init StrRep macro/function outside everything first
${StrRep}
${StrTok}

; SplitArg by Anders
; http://stackoverflow.com/a/19644470/145278
Function SplitArg
Exch $0 ; str
Push $1 ; inQ
Push $3 ; idx
Push $4 ; tmp
StrCpy $1 0
StrCpy $3 0
loop:
    StrCpy $4 $0 1 $3
    ${If} $4 == '"'
        ${If} $1 <> 0 
            StrCpy $0 $0 "" 1
            IntOp $3 $3 - 1
        ${EndIf}
        IntOp $1 $1 !
    ${EndIf}
    ${If} $4 == '' ; The end?
        StrCpy $1 0
        StrCpy $4 ' '
    ${EndIf} 
    ${If} $4 == ' '
    ${AndIf} $1 = 0
        StrCpy $4 $0 $3
        StrCpy $1 $4 "" -1
        ${IfThen} $1 == '"' ${|} StrCpy $4 $4 -1 ${|}
        killspace:
            IntOp $3 $3 + 1
            StrCpy $0 $0 "" $3
            StrCpy $1 $0 1
            StrCpy $3 0
            StrCmp $1 ' ' killspace
        Push $0 ; Remaining
        Exch 4
        Pop $0
        StrCmp $4 "" 0 moreleft
            Pop $4
            Pop $3
            Pop $1
            Return
        moreleft:
        Exch $4
        Exch 2
        Pop $1
        Pop $3
        Return
    ${EndIf}
    IntOp $3 $3 + 1
    Goto loop
FunctionEnd


!define fillArgv "!insertmacro fillArgv"
!macro fillArgv argline
  StrCpy $argv0 ""
  StrCpy $argv1 ""
  StrCpy $argv2 ""
  StrCpy $argv3 ""
  StrCpy $argv4 ""
  StrCpy $argv5 ""
  StrCpy $argv6 ""
  StrCpy $argv7 ""
  StrCpy $argv8 ""
  StrCpy $argv9 ""
  StrCpy $argvc 0
  Push "${argline}"
argloop:
  Call SplitArg
  Pop $R9
  StrCmp $R9 "" argdone
  ${Switch} $argvc
    ${Case} 0
      StrCpy $argv0 $R9
      ${Break}
    ${Case} 1
      StrCpy $argv1 $R9
      ${Break}
    ${Case} 2
      StrCpy $argv2 $R9
      ${Break}
    ${Case} 3
      StrCpy $argv3 $R9
      ${Break}
    ${Case} 4
      StrCpy $argv4 $R9
      ${Break}
    ${Case} 5
      StrCpy $argv5 $R9
      ${Break}
    ${Case} 6
      StrCpy $argv6 $R9
      ${Break}
    ${Case} 7
      StrCpy $argv7 $R9
      ${Break}
    ${Case} 8
      StrCpy $argv8 $R9
      ${Break}
    ${Case} 9
      StrCpy $argv9 $R9
      ${Break}
  ${EndSwitch}
  IntOp $argvc $argvc + 1
  goto argloop
argdone:
!macroend

!define processLine "!insertmacro processLine"
!macro processLine Line
  Push "${Line}"
  Call processLine
!macroend

Function processLine
	Var /GLOBAL line
	Var /GLOBAL argv
	Var /GLOBAL arg0
	Var /GLOBAL arg1
	Var /GLOBAL lineno

	StrCpy $lineno 1

	Pop $0
	${StrRep} $line "$0" "$\r" ""
	${StrRep} $line "$line" "$\n" ""
	${StrRep} $line "$line" "$$0" "$scriptname"
	${StrRep} $line "$line" "$$PROGRAMFILES" "$PROGRAMFILES"
	${StrRep} $line "$line" "$$PROGRAMFILES32" "$PROGRAMFILES32"
	${StrRep} $line "$line" "$$PROGRAMFILES64" "$PROGRAMFILES64"
	${StrRep} $line "$line" "$$COMMONFILES" "$COMMONFILES"
	${StrRep} $line "$line" "$$COMMONFILES32" "$COMMONFILES32"
	${StrRep} $line "$line" "$$COMMONFILES64" "$COMMONFILES64"
	${StrRep} $line "$line" "$$DESKTOP" "$DESKTOP"
	${StrRep} $line "$line" "$$EXEDIR" "$EXEDIR"
	${StrRep} $line "$line" "$$EXEFILE" "$EXEFILE"
	${StrRep} $line "$line" "$$EXEDIR" "$EXEDIR"
	${StrRep} $line "$line" "$$EXEPATH" "$EXEPATH"
	${StrRep} $line "$line" "$$WINDIR" "$WINDIR"
	${StrRep} $line "$line" "$$SYSDIR" "$SYSDIR"
	${StrRep} $line "$line" "$$TEMP" "$TEMP"
	${StrRep} $line "$line" "$$STARTMENU" "$STARTMENU"
	${StrRep} $line "$line" "$$SMPROGRAMS" "$SMPROGRAMS"
	${StrRep} $line "$line" "$$SMSTARTUP" "$SMSTARTUP"
	${StrRep} $line "$line" "$$QUICKLAUNCH" "$QUICKLAUNCH"
	${StrRep} $line "$line" "$$DOCUMENTS" "$DOCUMENTS"
	${StrRep} $line "$line" "$$SENDTO" "$SENDTO"
	${StrRep} $line "$line" "$$RECENT" "$RECENT"
	${StrRep} $line "$line" "$$FAVORITES" "$FAVORITES"
	${StrRep} $line "$line" "$$MUSIC" "$MUSIC"
	${StrRep} $line "$line" "$$PICTURES" "$PICTURES"
	${StrRep} $line "$line" "$$VIDEOS" "$VIDEOS"
	${StrRep} $line "$line" "$$FONTS" "$FONTS"
	${StrRep} $line "$line" "$$TEMPLATES" "$TEMPLATES"
	${StrRep} $line "$line" "$$APPDATA" "$APPDATA"
	${StrRep} $line "$line" "$$LOCALAPPDATA" "$LOCALAPPDATA"
	${StrRep} $line "$line" "$$INTERNET_CACHE" "$INTERNET_CACHE"
	${StrRep} $line "$line" "$$COOKIES" "$COOKIES"
	${StrRep} $line "$line" "$$HISTORY" "$HISTORY"
	${StrRep} $line "$line" "$$PROFILE" "$PROFILE"
	${StrRep} $line "$line" "$$ADMINTOOLS" "$ADMINTOOLS"
	${StrRep} $line "$line" "$$RESOURCES" "$RESOURCES"
	${StrRep} $line "$line" "$$CDBURN_AREA" "$CDBURN_AREA"
	${StrRep} $line "$line" "$$RESOURCES_LOCALIZED" "$RESOURCES_LOCALIZED"

	StrCpy $argv $line
	${StrTok} $arg0 $argv " " "0" ""
	${StrRep} $arg1 "$argv" "$arg0 " ""

	${fillArgv} $arg1

;	MessageBox MB_OK "line = '$line'$\narg0 = '$arg0'$\narg1 = '$arg1'$\n$\nargv0 = '$argv0'$\nargv1 = '$argv1'$\nargv2 = '$argv2'$\nargv3 = '$argv3'$\nargv4 = '$argv4'$\nargv5 = '$argv5'$\nargv6 = '$argv6'$\nargv7 = '$argv7'$\nargv8 = '$argv8'$\nargv9 = '$argv9'"
;	MessageBox MB_OK "line = '$line'$\narg0 = '$arg0'$\narg1 = '$arg1'"

	${Switch} $arg0
		${Case} '!wait'
			ExecWait "$arg1"
			${Break}
		${Case} '!exec'
			Exec "$arg1"
			${Break}
		${Case} '!msgbox'
			MessageBox MB_OK "$arg1"
			${Break}
		${Case} '!md'
			CreateDirectory "$arg1"
			${Break}
		${Case} '!copy'
;			${StrTok} $R1 $arg1 " " "0" ""
;			${StrTok} $R2 $arg1 " " "1" ""
			StrCpy $R1 $argv0
			StrCpy $R2 $argv1
			CopyFiles "$R1" "$R2"
			${Break}
		${Case} '!rd'
;			${StrTok} $R1 $arg1 " " "0" ""
;			${StrTok} $R2 $arg1 " " "1" ""
;			${StrTok} $R3 $arg1 " " "2" ""
			StrCpy $R1 $argv0
			StrCpy $R2 $argv1
			StrCpy $R3 $argv2
			${If} $R1 = '/r'
				${If} $R2 = '/REBOOTOK'
					RMDir /r /REBOOTOK "$R3"
				${Else}
					RMDir /r "$R2"
				${EndIf}
			${ElseIf} $R1 = '/REBOOTOK'
				${If} $R2 = '/r'
					RMDir /r /REBOOTOK "$R3"
				${Else}
					RMDir /REBOOTOK "$R2"
				${EndIf}
			${Else}
				RMDir "$R1"
			${EndIf}
			${Break}
		${Case} '!rename'
;			${StrTok} $R1 $arg1 " " "0" ""
;			${StrTok} $R2 $arg1 " " "1" ""
;			${StrTok} $R3 $arg1 " " "2" ""
			StrCpy $R1 $argv0
			StrCpy $R2 $argv1
			StrCpy $R3 $argv2
			${If} $R1 = '/REBOOTOK'
				Rename /REBOOTOK "$R2" "$R3"
			${Else}
				Rename "$R1" "$R2"
			${EndIf}
			${Break}
		${Case} '!del'
;			${StrTok} $R1 $arg1 " " "0" ""
;			${StrTok} $R2 $arg1 " " "1" ""
			StrCpy $R1 $argv0
			StrCpy $R2 $argv1
			${If} $R1 = '/REBOOTOK'
				Delete /REBOOTOK "$R2"
			${Else}
				Delete "$R1"
			${EndIf}
			${Break}
		${Case} '!open'
			ExecShell "open" "$arg1"
			${Break}
		${Case} '!regdll'
			RegDLL "$arg1"
			${Break}
		${Case} '!unregdll'
			UnRegDLL "$arg1"
			${Break}
	${EndSwitch}

	IntOp $lineno $lineno + 1
FunctionEnd

!define readFile "!insertmacro readFile"
!macro readFile File
  Push "${File}"
  Call readFile
!macroend

Function readFile
	Var /GLOBAL oline

	Exch $0
	StrCpy $scriptname $0
	FileOpen $4 "$scriptname" r
	FileRead $4 $oline
	${DoUntil} ${Errors}
		${processLine} $oline
		FileRead $4 $oline
	${LoopUntil} 1 = 0
	FileClose $4
FunctionEnd

Function .onInit
	SetSilent silent
	StrCpy $SetupName $EXEFILE -4
FunctionEnd

Section

	Var /GLOBAL args

	StrCpy $args $CMDLINE
	${GetParameters} $args

	${If} $args == ""
		${If} ${FileExists} `$EXEDIR\$SetupName.txt`
			${readFile} "$EXEDIR\$SetupName.txt"
		${EndIf}
       	${Else}
		${StrRep} $args "$args" '"' ""

		${If} ${FileExists} `$args`
			${readFile} "$args"
	       	${Else}
			${If} ${FileExists} `$EXEDIR\$args`
				${readFile} "$EXEDIR\$args"
			${Else}
				${processLine} $args
			${EndIf}
		${EndIf}
	${EndIf}

;	MessageBox MB_OK "CMDLINE = '$SetupName' '$args'"
SectionEnd
