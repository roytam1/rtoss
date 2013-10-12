!include "FileFunc.nsh"
!include "StrFunc.nsh"
!include "LogicLib.nsh"

Var SetupName

OutFile "shellrunner.exe"
Name $SetupName
Icon "gear_02.ico"

RequestExecutionLevel user

Var scriptname

; init StrRep macro/function outside everything first
${StrRep}
${StrTok}

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
			${StrTok} $R1 $arg1 " " "0" ""
			${StrTok} $R2 $arg1 " " "1" ""
			CopyFiles "$R1" "$R2"
			${Break}
		${Case} '!rd'
			${StrTok} $R1 $arg1 " " "0" ""
			${StrTok} $R2 $arg1 " " "1" ""
			${StrTok} $R3 $arg1 " " "2" ""
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
			${StrTok} $R1 $arg1 " " "0" ""
			${StrTok} $R2 $arg1 " " "1" ""
			${StrTok} $R3 $arg1 " " "2" ""
			${If} $R1 = '/REBOOTOK'
				Rename /REBOOTOK "$R2" "$R3"
			${Else}
				Rename "$R1" "$R2"
			${EndIf}
			${Break}
		${Case} '!del'
			${StrTok} $R1 $arg1 " " "0" ""
			${StrTok} $R2 $arg1 " " "1" ""
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

	Var /GLOBAL selfbase
	Var /GLOBAL args

	StrCpy $selfbase $EXEFILE -4
	StrCpy $args $CMDLINE
	${GetParameters} $args

	${If} $args == ""
		${If} ${FileExists} `$EXEDIR\$selfbase.txt`
			${readFile} "$EXEDIR\$selfbase.txt"
		${EndIf}
       	${Else}
		${StrRep} $args "$args" '"' ""

		${If} ${FileExists} `$args`
			${readFile} "$args"
	       	${Else}
			${If} ${FileExists} `$EXEDIR\$args`
				${readFile} "$EXEDIR\$args"
			${EndIf}
		${EndIf}
	${EndIf}

;	MessageBox MB_OK "CMDLINE = '$selfbase' '$args'"
SectionEnd
