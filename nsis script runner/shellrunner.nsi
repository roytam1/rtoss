!include "FileFunc.nsh"
!include "StrFunc.nsh"
!include "LogicLib.nsh"

!packhdr "$%TEMP%\exehead.tmp" 'upx --best "$%TEMP%\exehead.tmp"'

Var SetupName

OutFile "shellrunner.exe"
Name $SetupName
Icon "gear_02.ico"
SetCompressor LZMA

; Comment out next lines for ANSI build
Unicode true ; make a Unicode installer
;!define UTF16Script

RequestExecutionLevel user

Var scriptname
Var parseverbose
Var utf8script
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
Var lineno
Var skiplines

Var retval
Var var0
Var var1
Var var2
Var var3
Var var4
Var var5
Var var6
Var var7
Var var8
Var var9

!define RetvalToVar "!insertmacro RetvalToVar"
!macro RetvalToVar var
Push $retval
Pop ${var}
!macroend

!define VarToRetval "!insertmacro VarToRetval"
!macro VarToRetval var
Push ${var}
Pop $retval
!macroend

; init StrRep macro/function outside everything first
${StrRep}
${StrTok}

/*
!define CharToASCII "!insertmacro CharToASCII" 
 
!macro CharToASCII AsciiCode Character
  Push "${Character}"
  Call CharToASCII
  Pop "${AsciiCode}"
!macroend
 
Function CharToASCII
  Exch $0 ; given character
  Push $1 ; current character
  Push $2 ; current Ascii Code
 
  StrCpy $2 1 ; right from start
Loop:
  IntFmt $1 %c $2 ; Get character from current ASCII code
  ${If} $1 S== $0 ; case sensitive string comparison
     StrCpy $0 $2
     Goto Done
  ${EndIf}
  IntOp $2 $2 + 1
  StrCmp $2 255 0 Loop ; ascii from 1 to 255
  StrCpy $0 0 ; ASCII code wasn't found -> return 0
Done:         
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
*/

!define FileReadUTF8 "!insertmacro FileReadUTF8"
!macro FileReadUTF8 handle output
  Push "${handle}"
  Call FileReadUTF8
  Pop "${output}"
!macroend
Function FileReadUTF8
	Push $0
	Exch 
	Pop $0 ; handle
	Push $1 ; Out
	Push $2 ; read byte 1
	Push $3 ; read byte 2
 
	StrCpy $1 ""
	SetErrorLevel 0
	IfErrors done
read:	
	FileReadByte $0 $2
	${If} $2 = 0x0a
	${OrIf} $2 == ''
		Goto done
	${ElseIf} $2 = 0x0d
		Goto read
	${ElseIf} $2 < 0x80
		IntFmt $2 "%c" $2
		StrCpy $1 "$1$2"
	${ElseIf} $2 >= 0xc0
	${AndIf} $2 < 0xe0
		IntOp $2 $2 & 0x1f
		IntOp $2 $2 << 6
		FileReadByte $0 $3
		IntOp $3 $3 & 0x3f
		IntOp $2 $2 + $3
		IntFmt $2 "%c" $2
		StrCpy $1 "$1$2"
	${ElseIf} $2 >= 0xe0
	${AndIf} $2 < 0xf0
		IntOp $2 $2 & 0x0f
		IntOp $2 $2 << 6
		FileReadByte $0 $3
		IntOp $3 $3 & 0x3f
		IntOp $2 $2 + $3
		IntOp $2 $2 << 6
		FileReadByte $0 $3
		IntOp $3 $3 & 0x3f
		IntOp $2 $2 + $3
		IntFmt $2 "%c" $2
		StrCpy $1 "$1$2"
	${ElseIf} $2 >= 0xf0
	${AndIf} $2 < 0xf7
		IntOp $2 $2 & 0x07
		IntOp $2 $2 << 6
		FileReadByte $0 $3
		IntOp $3 $3 & 0x3f
		IntOp $2 $2 + $3
		IntOp $2 $2 << 6
		FileReadByte $0 $3
		IntOp $3 $3 & 0x3f
		IntOp $2 $2 + $3
		IntOp $2 $2 << 6
		FileReadByte $0 $3
		IntOp $3 $3 & 0x3f
		IntOp $2 $2 + $3
		IntFmt $2 "%c" $2
		StrCpy $1 "$1$2"
	${EndIf}

	Goto read

done:
	${If} $1 != ''
		ClearErrors
	${EndIf}
	Pop $3
	Pop $2
	Push $1
	Exch 
	Pop $1
	Exch 
	Pop $0
FunctionEnd


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
;    ${CharToASCII} $R9 $1
;    DetailPrint "'$0'|'$1'($R9)|'$3'|'$4'"
    ${If} $4 == '"'
        ${If} $1 <> 0 
            StrCpy $0 $0 "" 1
            IntOp $3 $3 - 1
        ${EndIf}
        IntOp $1 $1 !
    ${EndIf}
    StrLen $R8 $4
;    ${CharToASCII} $R9 $1
;    DetailPrint "x'$0'|'$1'($R9)|'$3'|'$4'[$R8]"
    ${If} $R8 = 0 ; The end?
        StrCpy $1 0
        StrCpy $4 ' '
    ${EndIf} 
    ${If} $4 == ' '
    ${AndIf} $1 = 0
        StrCpy $4 $0 $3
        StrCpy $1 $4 "" -1
;    ${CharToASCII} $R9 $1
;    DetailPrint "+'$0'|'$1'($R9)|'$3'|'$4'"
        ${IfThen} $1 == '"' ${|} StrCpy $4 $4 -1 ${|}
        killspace:
;    ${CharToASCII} $R9 $1
;    DetailPrint "++'$0'|'$1'($R9)|'$3'|'$4'"
            IntOp $3 $3 + 1
            StrCpy $0 $0 "" $3
            StrCpy $1 $0 1
            StrCpy $3 0
            StrCmp $1 ' ' killspace
        Push $0 ; Remaining
;    ${CharToASCII} $R9 $1
;    DetailPrint "-'$0'|'$1'($R9)|'$3'|'$4'"
        Exch 4
        Pop $0
        StrCmp $4 "" 0 moreleft
;    ${CharToASCII} $R9 $1
;    DetailPrint "*'$0'|'$1'($R9)|'$3'|'$4'"
            Pop $4
            Pop $3
            Pop $1
            Return
        moreleft:
;    ${CharToASCII} $R9 $1
;    DetailPrint "/'$0'|'$1'($R9)|'$3'|'$4'"
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
	Var /GLOBAL arg0len

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
	${StrRep} $line "$line" "$$retval" "$retval"
	${StrRep} $line "$line" "$$var0" "$var0"
	${StrRep} $line "$line" "$$var1" "$var1"
	${StrRep} $line "$line" "$$var2" "$var2"
	${StrRep} $line "$line" "$$var3" "$var3"
	${StrRep} $line "$line" "$$var4" "$var4"
	${StrRep} $line "$line" "$$var5" "$var5"
	${StrRep} $line "$line" "$$var6" "$var6"
	${StrRep} $line "$line" "$$var7" "$var7"
	${StrRep} $line "$line" "$$var8" "$var8"
	${StrRep} $line "$line" "$$var9" "$var9"

	StrCpy $argv $line
	${StrTok} $arg0 $argv " " "0" ""
	StrLen $arg0len "$arg0 "
	StrCpy $arg1 "$argv" "" $arg0len
;	${StrRep} $arg1 "$argv" "$arg0 " ""

	${fillArgv} $arg1

	${If} $argvc > 0
		${StrRep} $argv0 "$argv0" "$$empty" ""
	${EndIf}
	${If} $argvc > 1
		${StrRep} $argv1 "$argv1" "$$empty" ""
	${EndIf}
	${If} $argvc > 2
		${StrRep} $argv2 "$argv2" "$$empty" ""
	${EndIf}
	${If} $argvc > 3
		${StrRep} $argv3 "$argv3" "$$empty" ""
	${EndIf}
	${If} $argvc > 4
		${StrRep} $argv4 "$argv4" "$$empty" ""
	${EndIf}
	${If} $argvc > 5
		${StrRep} $argv5 "$argv5" "$$empty" ""
	${EndIf}
	${If} $argvc > 6
		${StrRep} $argv6 "$argv6" "$$empty" ""
	${EndIf}
	${If} $argvc > 7
		${StrRep} $argv7 "$argv7" "$$empty" ""
	${EndIf}
	${If} $argvc > 8
		${StrRep} $argv8 "$argv8" "$$empty" ""
	${EndIf}
	${If} $argvc > 9
		${StrRep} $argv9 "$argv9" "$$empty" ""
	${EndIf}

	StrCmp $line "" done
	${If} $parseverbose = 1
		MessageBox MB_OK "$lineno line = '$line'$\narg0 = '$arg0'$\narg1 = '$arg1'$\n$\nargvc = $argvc$\n$\nargv0 = '$argv0'$\nargv1 = '$argv1'$\nargv2 = '$argv2'$\nargv3 = '$argv3'$\nargv4 = '$argv4'$\nargv5 = '$argv5'$\nargv6 = '$argv6'$\nargv7 = '$argv7'$\nargv8 = '$argv8'$\nargv9 = '$argv9'"
	${EndIf}

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
			CopyFiles "$argv0" "$argv1"
			${Break}
		${Case} '!rd'
			${If} $argv0 == '/r'
				${If} $argv1 == '/REBOOTOK'
					RMDir /r /REBOOTOK "$argv2"
				${Else}
					RMDir /r "$argv1"
				${EndIf}
			${ElseIf} $argv0 == '/REBOOTOK'
				${If} $argv1 == '/r'
					RMDir /r /REBOOTOK "$argv2"
				${Else}
					RMDir /REBOOTOK "$argv1"
				${EndIf}
			${Else}
				RMDir "$argv0"
			${EndIf}
			${Break}
		${Case} '!rename'
			${If} $argv0 == '/REBOOTOK'
				Rename /REBOOTOK "$argv1" "$argv2"
			${Else}
				Rename "$argv0" "$argv1"
			${EndIf}
			${Break}
		${Case} '!del'
			${If} $argv0 == '/REBOOTOK'
				Delete /REBOOTOK "$argv1"
			${Else}
				Delete "$argv0"
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
		${Case} '!plugin'
			${Switch} $argvc
				${Case} 10
					Push $argv9
					Push $argv8
					Push $argv7
					Push $argv6
					Push $argv5
					Push $argv4
					Push $argv3
					Push $argv2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
				${Case} 9
					Push $argv8
					Push $argv7
					Push $argv6
					Push $argv5
					Push $argv4
					Push $argv3
					Push $argv2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
				${Case} 8
					Push $argv7
					Push $argv6
					Push $argv5
					Push $argv4
					Push $argv3
					Push $argv2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
				${Case} 7
					Push $argv6
					Push $argv5
					Push $argv4
					Push $argv3
					Push $argv2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
				${Case} 6
					Push $argv5
					Push $argv4
					Push $argv3
					Push $argv2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
				${Case} 5
					Push $argv4
					Push $argv3
					Push $argv2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
				${Case} 4
					Push $argv3
					Push $argv2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
				${Case} 3
					Push $argv2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
				${Case} 2
					CallInstDLL $argv0 $argv1
					Pop $retval
					${Break}
			${EndSwitch}
			${Break}
		${Case} '!set'
			${Switch} $argv0
				${Case} 'var0'
					StrCpy $var0 $argv1
					${Break}
				${Case} 'var1'
					StrCpy $var1 $argv1
					${Break}
				${Case} 'var2'
					StrCpy $var2 $argv1
					${Break}
				${Case} 'var3'
					StrCpy $var3 $argv1
					${Break}
				${Case} 'var4'
					StrCpy $var4 $argv1
					${Break}
				${Case} 'var5'
					StrCpy $var5 $argv1
					${Break}
				${Case} 'var6'
					StrCpy $var6 $argv1
					${Break}
				${Case} 'var7'
					StrCpy $var7 $argv1
					${Break}
				${Case} 'var8'
					StrCpy $var8 $argv1
					${Break}
				${Case} 'var9'
					StrCpy $var9 $argv1
					${Break}
			${EndSwitch}
			${Break}
		${Case} '!intfmt'
			IntFmt $retval $argv0 $argv1
			${Break}
		${Case} '!intop'
			${Switch} $argv1 
				${Case} '+'
					IntOp $retval $argv0 + $argv2
					${Break}
				${Case} '-'
					IntOp $retval $argv0 - $argv2
					${Break}
				${Case} '*'
					IntOp $retval $argv0 * $argv2
					${Break}
				${Case} '/'
					IntOp $retval $argv0 / $argv2
					${Break}
				${Case} '%'
					IntOp $retval $argv0 % $argv2
					${Break}
				${Case} '|'
					IntOp $retval $argv0 | $argv2
					${Break}
				${Case} '&'
					IntOp $retval $argv0 & $argv2
					${Break}
				${Case} '^'
					IntOp $retval $argv0 ^ $argv2
					${Break}
				${Case} '<<'
					IntOp $retval $argv0 << $argv2
					${Break}
				${Case} '||'
					IntOp $retval $argv0 || $argv2
					${Break}
				${Case} '&&'
					IntOp $retval $argv0 && $argv2
					${Break}
				${Case} '~'
					IntOp $retval $argv0 ~
					${Break}
				${Case} '!'
					IntOp $retval $argv0 !
					${Break}
			${EndSwitch}
			${Break}
		${Case} '!strrep'
			${StrRep} $retval $argv0 $argv1 $argv2
			${Break}
		${Case} '!strlen'
			StrLen $retval $argv0
			${Break}
		${Case} '!strcpy'
			${If} $argv1 == ''
				${If} $argv2 == ''
					StrCpy $retval "$argv0"
				${Else}
					StrCpy $retval "$argv0" "" "$argv2"
				${EndIf}
			${Else}
				${If} $argv2 = ''
					StrCpy $retval "$argv0" "$argv1"
				${Else}
					StrCpy $retval "$argv0" "$argv1" "$argv2"
				${EndIf}
			${EndIf}
			${Break}
		${Case} '!shellctx'
			${If} $argv0 == 'all'
				SetShellVarContext all
			${Else}
				SetShellVarContext current
			${EndIf}
			${Break}
		${Case} '!expendenv'
			ExpandEnvStrings $retval $argv0
			${Break}
		${Case} '!readenv'
			ReadEnvStr $retval $argv0
			${Break}
;		${Case} '!readregdword'
;			ReadRegDWORD $retval $argv0 $argv1 $argv2
;			${Break}
;		${Case} '!readregstr'
;			ReadRegStr $retval $argv0 $argv1 $argv2
;			${Break}
;		${Case} '!writeregstr'
;			WriteRegStr $argv0 $argv1 $argv2
;			${Break}
		${Case} '!readini'
			ReadINIStr $retval $argv0 $argv1 $argv2
			${Break}
		${Case} '!writeini'
			WriteINIStr $argv0 $argv1 $argv2 $argv3
			${Break}
		${Case} '!mklink'
			CreateShortcut $argv0 $argv1 $argv2 $argv3
			${Break}
		${Case} '!findwin'
			${If} $argvc = 4
				${If} $argv1 = ''
					FindWindow $retval $argv0 "" $argv2 $argv3
				${Else}
					FindWindow $retval $argv0 $argv1 $argv2 $argv3
				${EndIf}
			${ElseIf} $argvc = 3
				${If} $argv1 = ''
					FindWindow $retval $argv0 "" $argv2
				${Else}
					FindWindow $retval $argv0 $argv1 $argv2
				${EndIf}
			${ElseIf} $argvc = 2
				FindWindow $retval $argv0 $argv1
			${Else}
				FindWindow $retval $argv0
			${EndIf}
			${Break}
		${Case} '!showwin'
			ShowWindow $argv0 $argv1
			${Break}
		${Case} '!sendmsg'
			SendMessage $argv0 $argv1 $argv2 $argv3
			${Break}
		${Case} '!skip'
			IntOp $skiplines $argv0 + 0
			${Break}
		${Case} '!vartoret'
			${Switch} $argv0
				${Case} 'var0'
					${VarToRetval} $var0
					${Break}
				${Case} 'var1'
					${VarToRetval} $var1
					${Break}
				${Case} 'var2'
					${VarToRetval} $var2
					${Break}
				${Case} 'var3'
					${VarToRetval} $var3
					${Break}
				${Case} 'var4'
					${VarToRetval} $var4
					${Break}
				${Case} 'var5'
					${VarToRetval} $var5
					${Break}
				${Case} 'var6'
					${VarToRetval} $var6
					${Break}
				${Case} 'var7'
					${VarToRetval} $var7
					${Break}
				${Case} 'var8'
					${VarToRetval} $var8
					${Break}
				${Case} 'var9'
					${VarToRetval} $var9
					${Break}
			${EndSwitch}
			${Break}
		${Case} '!rettovar'
			${Switch} $argv0
				${Case} 'var0'
					${RetvalToVar} $var0
					${Break}
				${Case} 'var1'
					${RetvalToVar} $var1
					${Break}
				${Case} 'var2'
					${RetvalToVar} $var2
					${Break}
				${Case} 'var3'
					${RetvalToVar} $var3
					${Break}
				${Case} 'var4'
					${RetvalToVar} $var4
					${Break}
				${Case} 'var5'
					${RetvalToVar} $var5
					${Break}
				${Case} 'var6'
					${RetvalToVar} $var6
					${Break}
				${Case} 'var7'
					${RetvalToVar} $var7
					${Break}
				${Case} 'var8'
					${RetvalToVar} $var8
					${Break}
				${Case} 'var9'
					${RetvalToVar} $var9
					${Break}
			${EndSwitch}
			${Break}
		${Case} '!verbose'
			${If} $argv0 == 'on'
				StrCpy $parseverbose 1
			${Else}
				StrCpy $parseverbose 0
			${EndIf}
			${Break}
!ifndef UTF16Script
		${Case} '!utf8'
			${If} $argv0 == 'on'
				StrCpy $utf8script 1
			${Else}
				StrCpy $utf8script 0
			${EndIf}
			${Break}
!endif
	${EndSwitch}

done:
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
!ifdef UTF16Script
	FileReadUTF16LE $4 $oline
!else
	FileRead $4 $oline
!endif
	StrCpy $lineno 1
	StrCpy $skiplines 0
	${DoUntil} ${Errors}
		${If} $skiplines = 0
			${processLine} $oline
		${Else}
			IntOp $skiplines $skiplines - 1
		${EndIf}
		IntOp $lineno $lineno + 1
!ifdef UTF16Script
		FileReadUTF16LE $4 $oline
!else
		${If} $utf8script = 1
			${FileReadUTF8} $4 $oline
		${Else}
			FileRead $4 $oline
		${EndIf}
!endif
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
	StrCpy $parseverbose 0
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
